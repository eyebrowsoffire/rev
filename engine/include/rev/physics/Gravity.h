#pragma once

#include "rev/physics/Particle.h"
#include "rev/physics/PhysicsUnits.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace rev::physics {
class Gravity {
public:
  void setAcceleration(const Acceleration<glm::vec3> &acceleration);
  void addParticle(std::shared_ptr<Particle> &particle);
  void applyGravity(Time<float> time);

private:
  std::vector<std::shared_ptr<Particle>> _particles;
  Acceleration<glm::vec3> _acceleration;
};
} // namespace rev::physics