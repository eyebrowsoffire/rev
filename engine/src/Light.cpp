#include "rev/Light.h"

namespace rev
{
    Light::Light()
    : _baseColor(glm::vec3(1.0))
    , _position(glm::vec3(0.0))
    {
    }

    const glm::vec3& Light::getBaseColor() const
    {
        return _baseColor;
    }

    void Light::setBaseColor(const glm::vec3& color)
    {
        _baseColor = color;
    }

    const glm::vec3& Light::getPosition() const
    {
        return _position;
    }

    void Light::setPosition(const glm::vec3& position)
    {
        _position = position;
    }
}
