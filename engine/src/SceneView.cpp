#include "rev/SceneView.h"

#include "rev/Camera.h"
#include "rev/RenderStage.h"
#include "rev/Scene.h"

#include <iostream>

namespace rev {

namespace {

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

        uniform sampler2D fragPosition;
        uniform sampler2D normals;

        uniform sampler2D ambient;
        uniform sampler2D emissive;
        uniform sampler2D diffuse;
        uniform sampler2D specular;
        uniform sampler2D specularExponent;

        out vec4 fragColor;
        void main() 
        {
            vec3 normal = texture(normals, texCoord).rgb;
            vec3 diffuse = texture(diffuse, texCoord).rgb;

            vec3 fragmentPosition = texture(fragPosition, texCoord).rgb;
            vec3 lightVector = lightPosition - fragmentPosition;

            float attenuation = 1.0f / (1.0f + 0.01 * dot(lightVector, lightVector));
            float angleMultiplier = max(dot(normalize(lightVector), normalize(normal)), 0.0f);

            float specularExponent = texture(specularExponent, texCoord).r;
            vec3 specularCoefficient = texture(specular, texCoord).rgb;

            vec3 eyeVector = normalize(camPosition - fragmentPosition);
            vec3 reflectVector = normalize(reflect(lightVector, normalize(normal)));
            float specularComponent = max(dot(eyeVector, reflectVector), 0.0f);

            vec3 ambientLight = vec3(0.01f) * diffuse;
            vec3 diffuseLight = diffuse * lightBaseColor * attenuation * angleMultiplier;
            vec3 specularLight = (specularExponent > 0.01) 
                 ? lightBaseColor * specularCoefficient * pow(vec3(specularComponent) , vec3(specularExponent))
                 : vec3(0.0f);

            vec3 totalLight = diffuseLight + specularLight + ambientLight;
            
            fragColor = vec4(totalLight, 1.0f);
        }
    )fragmentShader";

constexpr glm::vec2 kFullScreenQuadVertices[] = {
    {-1.0f, -1.0},  {-1.0f, 1.0f}, {1.0f, 1.0f},

    {-1.0f, -1.0f}, {1.0f, 1.0f},  {1.0f, -1.0f}};

} // namespace

SceneView::SceneView() : _camera(std::make_shared<Camera>()) {
  _lightingProgram.buildWithSource(kDeferredVertexShader,
                                   kDeferredFragmentShader);
  {
    ProgramContext programContext(_lightingProgram);
    _lightPosition = _lightingProgram.getUniform<glm::vec3>("lightPosition");
    _lightBaseColor = _lightingProgram.getUniform<glm::vec3>("lightBaseColor");
    _camPosition = _lightingProgram.getUniform<glm::vec3>("camPosition");

    _lightingProgram.getUniform<GLint>("fragPosition").set(0);
    _lightingProgram.getUniform<GLint>("normals").set(1);

    _lightingProgram.getUniform<GLint>("ambient").set(2);
    _lightingProgram.getUniform<GLint>("emissive").set(3);
    _lightingProgram.getUniform<GLint>("diffuse").set(4);
    _lightingProgram.getUniform<GLint>("specular").set(5);
    _lightingProgram.getUniform<GLint>("specularExponent").set(6);
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
  _lightingStage.setOutputSize(outputSize);
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
    auto fbContext = _geometryStage.getRenderContext();

    glViewport(0, 0, _outputSize.width, _outputSize.height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    _scene->renderAllObjects(*_camera);
  }

  // Lighting pass
  {
    ProgramContext programContext(_lightingProgram);
    auto fbContext = _lightingStage.getRenderContext();

    _camPosition.set(_camera->getPosition());
    glViewport(0, 0, _outputSize.width, _outputSize.height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<WorldSpacePositionProperty>().getId());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<WorldSpaceNormalProperty>().getId());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<AmbientMaterialProperty>().getId());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<EmissiveMaterialProperty>().getId());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<DiffuseMaterialProperty>().getId());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<SpecularMaterialProperty>().getId());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(
        GL_TEXTURE_2D,
        _geometryStage.getOutputTexture<SpecularExponentProperty>().getId());

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    VertexArrayContext vaoContext(_fullScreenVao);

    _scene->renderAllLights(_lightPosition, _lightBaseColor);
  }
}

const Texture &SceneView::getOutputTexture() const {
  return _lightingStage.getOutputTexture<OutputColorProperty>();
}

} // namespace rev