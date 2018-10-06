#include "rev/SceneView.h"

#include "rev/Camera.h"
#include "rev/Scene.h"
#include "rev/RenderStage.h"

#include <OpenGL/gl3.h>

#include <iostream>

namespace rev {

namespace {

constexpr const char *kGeometryVertexShader = R"vertexShader(
        #version 330 core

        layout(location = 0) in vec3 vPosition;
        layout(location = 1) in vec3 vNormal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 fNormal;
        out vec3 fPosition;

        void main()
        {
            vec4 worldSpacePosition = model * vec4(vPosition, 1.0f);
            gl_Position = projection * view * worldSpacePosition;

            vec4 worldSpaceNormal = model * vec4(vNormal, 0.0f);
            fNormal = normalize(worldSpaceNormal.xyz);
            fPosition = worldSpacePosition.xyz;
        }
    )vertexShader";

constexpr const char *kGeometryFragmentShader = R"fragmentShader(
        #version 330 core

        in vec3 fNormal;
        in vec3 fPosition;

        uniform vec3 faceColor;

        layout(location = 0) out vec3 normal;
        layout(location = 1) out vec3 baseColor;
        layout(location = 2) out vec3 position;

        void main() 
        {
            normal = fNormal;
            baseColor = faceColor;
            position = fPosition;
        }
    )fragmentShader";

constexpr const char *kDeferredVertexShader = R"vertexShader(
    #version 330 core

    layout(location = 0) in vec2 vPosition;

    out vec2 texCoord;

    void main()
    {
        gl_Position = vec4(vPosition, 0.0f, 1.0f);
        texCoord = (vPosition + vec2(1.0f)) / 2.0f;
    }

    )vertexShader";

constexpr const char *kDeferredFragmentShader = R"fragmentShader(
        #version 330 core

        in vec2 texCoord;

        uniform vec3 lightPosition;
        uniform vec3 lightBaseColor;
        uniform vec3 camPosition;

        uniform sampler2D normals;
        uniform sampler2D materialBaseColor;
        uniform sampler2D fragPosition;

        out vec4 fragColor;
        void main() 
        {
            vec3 normal = texture(normals, texCoord).rgb;
            vec3 baseColor = texture(materialBaseColor, texCoord).rgb;

            vec3 fragmentPosition = texture(fragPosition, texCoord).rgb;
            vec3 lightVector = lightPosition - fragmentPosition;

            float attenuation = 1.0f / (1.0f + 0.01 * dot(lightVector, lightVector));
            float angleMultiplier = max(dot(normalize(lightVector), normalize(normal)), 0.0f);

            float shininess = 200.0f;

            vec3 eyeVector = normalize(camPosition - fragmentPosition);
            vec3 halfwayVector = normalize(eyeVector + normalize(lightVector));
            float specularComponent = max(dot(halfwayVector, normal), 0.0f);

            vec3 ambientLight = vec3(0.01f) * baseColor;
            vec3 diffuseLight = baseColor * lightBaseColor * attenuation * angleMultiplier;
            vec3 specularLight = lightBaseColor * pow(specularComponent, shininess);

            vec3 totalLight = diffuseLight + specularLight + ambientLight;
            
            fragColor = vec4(totalLight, 1.0f);
        }
    )fragmentShader";

constexpr glm::vec2 kFullScreenQuadVertices[] = {
    {-1.0f, -1.0},  {-1.0f, 1.0f}, {1.0f, 1.0f},

    {-1.0f, -1.0f}, {1.0f, 1.0f},  {1.0f, -1.0f}};

} // namespace

SceneView::SceneView() : _camera(std::make_shared<Camera>()) {
  _geometryProgram.buildWithSource(kGeometryVertexShader,
                                   kGeometryFragmentShader);
  _lightingProgram.buildWithSource(kDeferredVertexShader,
                                   kDeferredFragmentShader);
  {
    ProgramContext programContext(_geometryProgram);
    _model = _geometryProgram.getUniform<glm::mat4>("model");
    _view = _geometryProgram.getUniform<glm::mat4>("view");
    _projection = _geometryProgram.getUniform<glm::mat4>("projection");
    _faceBaseColor = _geometryProgram.getUniform<glm::vec3>("faceColor");
  }

  {
    ProgramContext programContext(_lightingProgram);
    _lightPosition = _lightingProgram.getUniform<glm::vec3>("lightPosition");
    _lightBaseColor = _lightingProgram.getUniform<glm::vec3>("lightBaseColor");
    _camPosition = _lightingProgram.getUniform<glm::vec3>("camPosition");

    _lightingProgram.getUniform<GLint>("normals").set(0);
    _lightingProgram.getUniform<GLint>("materialBaseColor").set(1);
    _lightingProgram.getUniform<GLint>("fragPosition").set(2);
  }

  {
    ReadWriteFrameBufferContext fbContext(_outputFramebuffer);
    Texture2DContext texContext(_outputTexture);

    texContext.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    texContext.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    fbContext.setTextureAttachment(GL_COLOR_ATTACHMENT0, _outputTexture);
  }
  VertexArrayContext vaoContext(_fullScreenVao);
  ArrayBufferContext bufferContext(_fullScreenVertexBuffer);

  bufferContext.bindData(gsl::span<const glm::vec2>(kFullScreenQuadVertices),
                         GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
}

void SceneView::setScene(std::shared_ptr<Scene> scene) {
  _scene = std::move(scene);
}

void SceneView::setOutputSize(const RectSize<GLsizei> &outputSize) {
  if (_outputSize.width == outputSize.width &&
      _outputSize.height == outputSize.height) {
    return;
  }
  _outputSize = outputSize;
  _geometryStage.setOutputSize(outputSize);

  {
    Texture2DContext texContext(_outputTexture);
    texContext.setImage(0, GL_SRGB8_ALPHA8, _outputSize.width,
                        _outputSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                        nullptr);
  }
}

const RectSize<GLsizei> &SceneView::getOutputSize() const {
  return _outputSize;
}

const std::shared_ptr<Camera> &SceneView::getCamera() const { return _camera; }

void SceneView::render() {
  if (!_scene) {
    return;
  }

  // Geometry pass
  {
    ProgramContext programContext(_geometryProgram);
    auto fbContext = _geometryStage.getRenderContext();

    glViewport(0, 0, _outputSize.width, _outputSize.height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    _view.set(_camera->getViewMatrix());
    _projection.set(_camera->getProjectionMatrix());

    _scene->renderAllObjects(_model, _faceBaseColor);
  }

  // Lighting pass
  {
    ProgramContext programContext(_lightingProgram);
    ReadWriteFrameBufferContext fbContext(_outputFramebuffer);

    _camPosition.set(_camera->getPosition());
    glViewport(0, 0, _outputSize.width, _outputSize.height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _geometryStage.getOutputTexture<WorldSpaceNormalProperty>().getId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _geometryStage.getOutputTexture<BaseColorProperty>().getId());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _geometryStage.getOutputTexture<WorldSpacePositionProperty>().getId());

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    VertexArrayContext vaoContext(_fullScreenVao);

    _scene->renderAllLights(_lightPosition, _lightBaseColor);
  }
}

const Texture &SceneView::getOutputTexture() const { return _outputTexture; }

} // namespace rev