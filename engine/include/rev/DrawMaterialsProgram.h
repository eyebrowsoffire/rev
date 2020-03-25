#pragma once

#include "rev/MaterialProperties.h"
#include "rev/gl/ProgramResource.h"
#include "rev/shaders/Shader.h"

#include <glm/glm.hpp>

namespace rev {

struct DrawMaterialsVertexComponent {
    static inline const std::array inputs{
        IndexedShaderVariable::make<glm::vec3>("vPosition", 0),
        IndexedShaderVariable::make<glm::vec3>("vNormal", 1),
    };

    static inline const std::array outputs{
        ShaderVariable::make<glm::vec3>("fPosition"),
        ShaderVariable::make<glm::vec3>("fNormal"),
    };

    static inline const std::array uniforms{
        ShaderVariable::make<glm::mat4>("model"),
        ShaderVariable::make<glm::mat4>("view"),
        ShaderVariable::make<glm::mat4>("projection"),
    };

    static constexpr std::array<ShaderFunction, 0> requiresFunctions{};
    static constexpr std::array fulfillsFunctions{ ShaderFunction{ "main" } };

    static constexpr std::string_view source{ R"vertexShader(
        layout(location = 0) in vec3 vPosition;
        layout(location = 1) in vec3 vNormal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 fPosition;
        out vec3 fNormal;

        void main()
        {
            vec4 viewSpacePosition = view * model * vec4(vPosition, 1.0f);
            gl_Position = projection * viewSpacePosition;

            vec4 viewSpaceNormal = view * model * vec4(vNormal, 0.0f);
            fNormal = normalize(viewSpaceNormal.xyz);
            fPosition = viewSpacePosition.xyz;
        }
)vertexShader" };
};

struct DrawMaterialsFragmentComponent {
    static inline const std::array inputs{
        ShaderVariable::make<glm::vec3>("fPosition"),
        ShaderVariable::make<glm::vec3>("fNormal"),
    };

    static inline const std::array outputs{
        IndexedShaderVariable::make<glm::vec3>("position", 0),
        IndexedShaderVariable::make<glm::vec3>("normal", 1),
        IndexedShaderVariable::make<glm::vec3>("ambient", 2),
        IndexedShaderVariable::make<glm::vec3>("emissive", 3),
        IndexedShaderVariable::make<glm::vec3>("diffuse", 4),
        IndexedShaderVariable::make<glm::vec3>("specular", 5),
        IndexedShaderVariable::make<float>("specularExponent", 6),
    };

    static inline const std::array uniforms{
        ShaderVariable::make<glm::vec3>("fAmbient"),
        ShaderVariable::make<glm::vec3>("fEmissive"),
        ShaderVariable::make<glm::vec3>("fDiffuse"),
        ShaderVariable::make<glm::vec3>("fSpecular"),
        ShaderVariable::make<float>("fSpecularExponent"),
    };

    static inline const std::array<ShaderFunction, 0> requiresFunctions{};
    static inline const std::array fulfillsFunctions{ ShaderFunction{ "main" } };

    static constexpr std::string_view source{ R"fragmentShader(
        in vec3 fPosition;
        in vec3 fNormal;
 
        uniform vec3 fAmbient;
        uniform vec3 fEmissive;
        uniform vec3 fDiffuse;
        uniform vec3 fSpecular;
        uniform float fSpecularExponent;

        layout(location = 0) out vec3 position;
        layout(location = 1) out vec3 normal;

        layout(location = 2) out vec3 ambient;
        layout(location = 3) out vec3 emissive;
        layout(location = 4) out vec3 diffuse;
        layout(location = 5) out vec3 specular;
        layout(location = 6) out float specularExponent;

        void main() 
        {
            position = fPosition;
            normal = fNormal;

            ambient = fAmbient;
            emissive = fEmissive;
            diffuse = fDiffuse;
            specular = fSpecular;
            specularExponent = fSpecularExponent;
        }
    )fragmentShader" };
};

} // namespace rev
