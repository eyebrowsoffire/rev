#pragma once

#include <glm/glm.hpp>

namespace rev {

class Light {
public:
  Light();

  const glm::vec3 &getBaseColor() const;
  void setBaseColor(const glm::vec3 &color);

  const glm::vec3 &getPosition() const;
  void setPosition(const glm::vec3 &position);

private:
  glm::vec3 _baseColor;
  glm::vec3 _position;
};

} // namespace rev