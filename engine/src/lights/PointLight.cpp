#include "rev/lights/PointLight.h"

#include "rev/gl/ProgramResource.h"

namespace rev {

namespace {
    constexpr glm::vec2 kFullScreenQuadVertices[] = {
        { -1.0f, -1.0 },
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },

        { -1.0f, -1.0f },
        { 1.0f, 1.0f },
        { 1.0f, -1.0f },
    };
}

class PointLightProgram {
public:
    PointLightProgram(ProgramResource resource)
    : _resource(std::move(resource))
    {
        lightPosition = _resource.getUniform<glm::vec3>("lightPosition");
        lightBaseColor = _resource.getUniform<glm::vec3>("lightBaseColor");
        camPosition = _resource.getUniform<glm::vec3>("camPosition");

        fragPosition = _resource.getUniform<GLint>("fragPosition");
        normals = _resource.getUniform<GLint>("normals");

        ambient = _resource.getUniform<GLint>("ambient");
        emissive = _resource.getUniform<GLint>("emissive");
        diffuse = _resource.getUniform<GLint>("diffuse");
        specular = _resource.getUniform<GLint>("specular");
        specularExponent = _resource.getUniform<GLint>("specularExponent");
    }

    ProgramContext prepareContext() { return ProgramContext(_resource); }

    Uniform<glm::vec3> lightPosition;
    Uniform<glm::vec3> lightBaseColor;
    Uniform<glm::vec3> camPosition;

    Uniform<GLint> fragPosition;
    Uniform<GLint> normals;

    Uniform<GLint> ambient;
    Uniform<GLint> emissive;
    Uniform<GLint> diffuse;
    Uniform<GLint> specular;
    Uniform<GLint> specularExponent;

    struct Source {
        static std::string_view getVertexSource()
        {
            return R"vertexShader(
#version 330 core
layout(location = 0) in vec2 vPosition;
out vec2 texCoord;
void main()
{
    gl_Position = vec4(vPosition, 0.0f, 1.0f);
    texCoord = (vPosition + vec2(1.0f)) / 2.0f;
}
)vertexShader";
        }

        static std::string_view getFragmentSource()
        {
            return R"fragmentShader(
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
    float angleMultiplier = dot(normalize(lightVector), normalize(normal));
    if (angleMultiplier < 0.0f)
    {
        discard;
    }

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
        }
    };

private:
    ProgramResource _resource;
};

const glm::vec3& PointLight::getBaseColor() const { return _baseColor; }
void PointLight::setBaseColor(const glm::vec3& color) { _baseColor = color; }

const glm::vec3& PointLight::getPosition() const { return _position; }
void PointLight::setPosition(const glm::vec3& position) { _position = position; }

const glm::vec3& PointLight::getFalloffCoefficients() const { return _falloffCoefficients; }
void PointLight::setFalloffCoefficients(const glm::vec3& coefficients)
{
    _falloffCoefficients = coefficients;
}

PointLightModel::PointLightModel(ProgramFactory& factory)
    : _program(factory.getProgram<PointLightProgram>())
{
    {
        VertexArrayContext context(_vao);
        context.setBuffer<GL_ARRAY_BUFFER>(_vertices);
        context.bindBufferData<GL_ARRAY_BUFFER>(gsl::span(kFullScreenQuadVertices), GL_STATIC_DRAW);

        context.setupVertexAttribute<glm::vec2>(0, 2, sizeof(glm::vec2));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
    }

    {
        auto context = _program->prepareContext();
        _program->fragPosition.set(0);
        _program->normals.set(1);

        _program->ambient.set(2);
        _program->emissive.set(3);
        _program->diffuse.set(4);
        _program->specular.set(5);
        _program->specularExponent.set(6);
    }
}

void PointLightModel::render(Camera& camera, const std::vector<std::shared_ptr<PointLight>>& lights)
{
    if (lights.empty()) {
        return;
    }

    auto programContext = _program->prepareContext();
    _program->camPosition.set(camera.getPosition());

    VertexArrayContext vaoContext(_vao);
    for (const auto light : lights) {
        _program->lightPosition.set(light->getPosition());
        _program->lightBaseColor.set(light->getBaseColor());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
}