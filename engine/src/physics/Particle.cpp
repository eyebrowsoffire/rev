#include "rev/physics/Particle.h"

namespace rev::physics
{
void Particle::setMass(Mass<float> mass) { _mass = mass; }

Mass<float> Particle::getMass() const { return _mass; }

void Particle::addImpulse(const Momentum<glm::vec3> &impulse)
{
    _accumulatedImpulse += impulse;
}

void Particle::flushImpulses()
{
    _velocity += _accumulatedImpulse / _mass;
    _accumulatedImpulse = glm::vec3(0.0f);
}

void Particle::updatePosition(Time<float> elapsedTime)
{
    _position += _velocity * elapsedTime;
}

const Distance<glm::vec3> &Particle::getPosition() const 
{
    return _position;
}

} // namespace rev::physics
