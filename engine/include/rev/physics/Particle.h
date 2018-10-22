#pragma once

#include "rev/physics/PhysicsUnits.h"

#include <glm/glm.hpp>

namespace rev::physics
{
    class PhysicsParticle
    {
    public:
    private:
        Force<glm::vec3> _accumulatedForce;
        Velocity<glm::vec3> _velocity;
        Distance<glm::vec3> _position;
    };
}