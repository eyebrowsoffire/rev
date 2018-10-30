#pragma once

#include "rev/physics/PhysicsUnits.h"

#include <glm/glm.hpp>

namespace rev::physics
{
    class Particle
    {
    public:
        void setMass(Mass<float> mass);
        Mass<float> getMass() const;
        void addImpulse(const Momentum<glm::vec3> &impulse);

        void flushImpulses();
        void updatePosition(Time<float> elapsedTime);

        const Distance<glm::vec3> &getPosition() const;

    private:
        Momentum<glm::vec3> _accumulatedImpulse;
        Velocity<glm::vec3> _velocity;
        Distance<glm::vec3> _position;
        Mass<float> _mass;
    };
}