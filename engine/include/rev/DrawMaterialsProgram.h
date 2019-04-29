#pragma once

#include "rev/gl/ProgramResource.h"

#include <glm/glm.hpp>

namespace rev {
class DrawMaterialsProgram {
public:
  DrawMaterialsProgram(ProgramResource resource)
      : _programResource(std::move(resource)) {
    model = _programResource.getUniform<glm::mat4>("model");
    view = _programResource.getUniform<glm::mat4>("view");
    projection = _programResource.getUniform<glm::mat4>("projection");

    ambient = _programResource.getUniform<glm::vec3>("fAmbient");
    emissive = _programResource.getUniform<glm::vec3>("fEmissive");
    diffuse = _programResource.getUniform<glm::vec3>("fDiffuse");
    specular = _programResource.getUniform<glm::vec3>("fSpecular");
    specularExponent = _programResource.getUniform<float>("fSpecularExponent");
  }

  ProgramContext prepareContext() { return ProgramContext(_programResource); }

  Uniform<glm::mat4> model;
  Uniform<glm::mat4> view;
  Uniform<glm::mat4> projection;

  Uniform<glm::vec3> ambient;
  Uniform<glm::vec3> emissive;
  Uniform<glm::vec3> diffuse;
  Uniform<glm::vec3> specular;
  Uniform<float> specularExponent;

  struct Source {
    static std::string_view getVertexSource() {
      return R"vertexShader(
                #version 330 core

                layout(location = 0) in vec3 vPosition;
                layout(location = 1) in vec3 vNormal;

                uniform mat4 model;
                uniform mat4 view;
                uniform mat4 projection;

                out vec3 fPosition;
                out vec3 fNormal;

                void main()
                {
                    vec4 worldSpacePosition = model * vec4(vPosition, 1.0f);
                    gl_Position = projection * view * worldSpacePosition;

                    vec4 worldSpaceNormal = model * vec4(vNormal, 0.0f);
                    fNormal = normalize(worldSpaceNormal.xyz);
                    fPosition = worldSpacePosition.xyz;
                }
            )vertexShader";
    }

    static std::string_view getFragmentSource() {
      return R"fragmentShader(
                #version 330 core

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
            )fragmentShader";
    }
  };

private:
  ProgramResource _programResource;
};

} // namespace rev
