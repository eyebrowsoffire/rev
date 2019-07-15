#pragma once

#include "rev/gl/ProgramResource.h"

#include <array>
#include <string_view>

namespace rev {
constexpr std::string_view kVertexShader = R"vertexShader(
#version 330 core
layout(location = 0) in vec2 vPosition;
out vec2 texCoord;
void main()
{
    gl_Position = vec4(vPosition, 0.0f, 1.0f);
    texCoord = (vPosition + vec2(1.0f)) / 2.0f;
}
)vertexShader";

constexpr std::string_view kFragmentSharedDeclarations = R"sharedDecl(
#version 330 core
in vec2 texCoord;

uniform sampler2D fragPosition;
uniform sampler2D normals;

uniform sampler2D ambient;
uniform sampler2D emissive;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D specularExponent;

out vec4 fragColor;

struct RayInfo
{
    float attenuation;
    vec3 lightVector;
};
)sharedDecl";

constexpr std::string_view kPointLightComponents = R"pointLight(
uniform vec3 lightPosition;
uniform vec3 lightBaseColor;
uniform vec3 falloffCoefficients;

float getAttenuation(float distance)
{
    float constant = falloffCoefficients[0];
    float linear = falloffCoefficients[1] * distance;
    float quadratic = falloffCoefficients[2] * distance * distance;
    return 1.0f / (constant + linear + quadratic);
}

RayInfo getRayInfo(vec3 fragmentPosition)
{
    RayInfo info;

    vec3 lightVector = fragmentPosition - lightPosition;
    info.attenuation = getAttenuation(length(lightVector));
    info.lightVector = normalize(lightVector);

    return info;
}
)pointLight";

constexpr std::string_view kDirectionalLightComponents = R"dirLight(
uniform vec3 lightDirection;
uniform vec3 lightBaseColor;

RayInfo getRayInfo(vec3 fragmentPosition)
{
    RayInfo info;
    info.attenuation = 1.0f;
    info.lightVector = lightDirection;
    return info;
}

)dirLight";

constexpr std::string_view kFragmentMain = R"fragMain(
void main() 
{
    vec3 normal = texture(normals, texCoord).rgb;
    vec3 diffuse = texture(diffuse, texCoord).rgb;
    vec3 fragmentPosition = texture(fragPosition, texCoord).rgb;

    RayInfo rayInfo = getRayInfo(fragmentPosition);
    float angleMultiplier = dot(-rayInfo.lightVector, normal);
    bool isValid = (angleMultiplier > 0.0f) && (rayInfo.attenuation > 0.0f);
    if (!isValid)
    {
        discard;
    }

    float specularExponent = texture(specularExponent, texCoord).r;
    vec3 specularCoefficient = texture(specular, texCoord).rgb;
    vec3 eyeVector = normalize(-fragmentPosition);
    vec3 reflectVector = normalize(reflect(rayInfo.lightVector, normal));

    float specularComponent = max(dot(eyeVector, reflectVector), 0.0f);
    vec3 ambientLight = vec3(0.01f) * diffuse;
    vec3 diffuseLight = diffuse * lightBaseColor * angleMultiplier;
    vec3 specularLight = (specularExponent > 0.01) 
         ? lightBaseColor * specularCoefficient * pow(vec3(specularComponent), vec3(specularExponent))
         : vec3(0.0f);
    vec3 totalLight = rayInfo.attenuation * (diffuseLight + specularLight) + ambientLight;
    
    fragColor = vec4(totalLight, 1.0f);
}
)fragMain";

class LightProgram {
public:
    LightProgram(ProgramResource resource)
        : _resource(std::move(resource))
    {
        fragPosition = _resource.getUniform<GLint>("fragPosition");
        normals = _resource.getUniform<GLint>("normals");

        ambient = _resource.getUniform<GLint>("ambient");
        emissive = _resource.getUniform<GLint>("emissive");
        diffuse = _resource.getUniform<GLint>("diffuse");
        specular = _resource.getUniform<GLint>("specular");
        specularExponent = _resource.getUniform<GLint>("specularExponent");
    }

    ProgramContext prepareContext() { return ProgramContext(_resource); }

    Uniform<GLint> fragPosition;
    Uniform<GLint> normals;

    Uniform<GLint> ambient;
    Uniform<GLint> emissive;
    Uniform<GLint> diffuse;
    Uniform<GLint> specular;
    Uniform<GLint> specularExponent;

protected:
    ProgramResource _resource;
};

class PointLightProgram : public LightProgram {
public:
    PointLightProgram(ProgramResource resource)
        : LightProgram(std::move(resource))
    {
        lightPosition = _resource.getUniform<glm::vec3>("lightPosition");
        lightBaseColor = _resource.getUniform<glm::vec3>("lightBaseColor");
        falloffCoefficients = _resource.getUniform<glm::vec3>("falloffCoefficients");
    }

    Uniform<glm::vec3> lightPosition;
    Uniform<glm::vec3> lightBaseColor;
    Uniform<glm::vec3> falloffCoefficients;

    struct Source {
        static std::string_view getVertexSource() { return kVertexShader; }

        static std::array<std::string_view, 3> getFragmentSource()
        {
            return {
                kFragmentSharedDeclarations,
                kPointLightComponents,
                kFragmentMain,
            };
        }
    };
};

class DirectionalLightProgram : public LightProgram {
public:
    DirectionalLightProgram(ProgramResource resource)
        : LightProgram(std::move(resource))
    {
        lightDirection = _resource.getUniform<glm::vec3>("lightDirection");
        lightBaseColor = _resource.getUniform<glm::vec3>("lightBaseColor");
    }

    struct Source {
        static std::string_view getVertexSource() { return kVertexShader; }

        static std::array<std::string_view, 3> getFragmentSource()
        {
            return {
                kFragmentSharedDeclarations,
                kDirectionalLightComponents,
                kFragmentMain,
            };
        }
    };

    Uniform<glm::vec3> lightDirection;
    Uniform<glm::vec3> lightBaseColor;
};
}