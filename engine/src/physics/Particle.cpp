#include "rev/physics/Particle.h"

#include <glm/ext.hpp>
#include <gsl/gsl_assert>

namespace rev::physics {
void Particle::setMass(Mass<float> mass) { _mass = mass; }

Mass<float> Particle::getMass() const { return _mass; }

void Particle::setDamping(float damping) { _damping = damping; }

float Particle::getDamping() const { return _damping; }

void Particle::addImpulse(const Momentum<glm::vec3>& impulse)
{
    Expects(!glm::any(glm::isnan(impulse.getValue())));
    Expects(!glm::any(glm::isinf(impulse.getValue())));
    _accumulatedImpulse += impulse;
}

void Particle::flushImpulses()
{
    _velocity += _accumulatedImpulse / _mass;
    _accumulatedImpulse = glm::vec3(0.0f);
}

void Particle::applyDamping(Time<float> elapsedTime)
{
    // Modeled as continuous decay.
    _velocity = _velocity * std::exp(-1.0f * _damping * elapsedTime.getValue());
}

void Particle::updatePosition(Time<float> elapsedTime) { _position += _velocity * elapsedTime; }

const Distance<glm::vec3>& Particle::getPosition() const { return _position; }

void Particle::setPosition(const Distance<glm::vec3>& position) { _position = position; }

const Velocity<glm::vec3>& Particle::getVelocity() const { return _velocity; }

void Particle::setVelocity(const Velocity<glm::vec3>& velocity) { _velocity = velocity; }

} // namespace rev::physics
