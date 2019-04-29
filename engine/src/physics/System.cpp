#include "rev/physics/System.h"

namespace rev::physics {

System::System() : _gravity(std::make_shared<Gravity>()) {}

void System::tick(Environment &environment, Duration elapsedTime) {
  auto physicsTime = durationToPhysicsTime(elapsedTime);
  _gravity->applyGravity(physicsTime);

  for (const auto &particle : _particles) {
    particle->flushImpulses();
    particle->updatePosition(physicsTime);
  }
}

void System::kill(Environment &environment) {}

std::shared_ptr<Particle> System::addParticle() {
  auto particle = std::make_shared<Particle>();
  _particles.push_back(particle);

  return std::move(particle);
}

std::shared_ptr<Gravity> System::getGravity() const { return _gravity; }

} // namespace rev::physics