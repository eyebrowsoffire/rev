#pragma once

#include "rev/physics/PhysicsUnits.h"

#include <glm/glm.hpp>

namespace rev::physics {
class Particle {
public:
    void setMass(Mass<float> mass);
    Mass<float> getMass() const;

    void setDamping(float damping);
    float getDamping() const;
    
    void addImpulse(const Momentum<glm::vec3>& impulse);

    void flushImpulses();
    void applyDamping(Time<float> elapsedTime);
    void updatePosition(Time<float> elapsedTime);

    const Distance<glm::vec3>& getPosition() const;
    void setPosition(const Distance<glm::vec3>& position);

    const Velocity<glm::vec3>& getVelocity() const;
    void setVelocity(const Velocity<glm::vec3>& velocity);

private:
    Momentum<glm::vec3> _accumulatedImpulse;
    Velocity<glm::vec3> _velocity;
    Distance<glm::vec3> _position;
    Mass<float> _mass;
    float _damping = 0.0f;
};
} // namespace rev::physics