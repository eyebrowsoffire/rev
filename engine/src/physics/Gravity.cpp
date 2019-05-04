#include "rev/physics/Gravity.h"

namespace rev::physics {
void Gravity::setAcceleration(const Acceleration<glm::vec3>& acceleration)
{
    _acceleration = acceleration;
}

void Gravity::addParticle(std::shared_ptr<Particle>& particle)
{
    _particles.push_back(particle);
}

void Gravity::applyGravity(Time<float> time)
{
    for (const auto& particle : _particles) {
        auto deltaV = _acceleration * time;
        particle->addImpulse(deltaV * particle->getMass());
    }
}

} // namespace rev::physics