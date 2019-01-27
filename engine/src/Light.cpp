#include "rev/Light.h"

namespace rev
{
    // Light
    Light::Light()
    : _baseColor(glm::vec3(1.0))
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

    // PointLight
    PointLight::PointLight()
    : _position(glm::vec3(0.0))
    {
    }

    const glm::vec3& PointLight::getPosition() const
    {
        return _position;
    }

    void PointLight::setPosition(const glm::vec3& position)
    {
        _position = position;
    }

    // DirectionalLight
    DirectionalLight::DirectionalLight()
    : _direction(glm::vec3(0.0, -1.0, 0.0))
    {
    }

    const glm::vec3& DirectionalLight::getDirection() const 
    {
        return _direction;
    }

    void DirectionalLight::setDirection(const glm::vec3& direction)
    {
        _direction = direction;
    }
}
