#pragma once

#include <glm/glm.hpp>

namespace rev {
struct MaterialProperties {
  glm::vec3 ambientColor;
  glm::vec3 emissiveColor;
  glm::vec3 specularColor;
  glm::vec3 diffuseColor;
  float specularExponent;
};
} // namespace rev
