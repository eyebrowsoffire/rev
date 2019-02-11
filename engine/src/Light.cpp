#include "rev/Light.h"

using namespace std;
using namespace glm;

namespace rev
{
    // Light
    Light::Light(Type type)
    : _baseColor(0.f), _type(type)
    {
    }

    Light::Type Light::getType() const
    {
        return _type;
    } 

    const glm::vec3& Light::getBaseColor() const
    {
        return _baseColor;
    }

    void Light::setBaseColor(const glm::vec3& baseColor)
    {
        _baseColor = baseColor;
    }

    const glm::vec3& Light::getPosition() const
    {
        assert(_position.has_value());
        return *_position;
    }

    void Light::setPosition(const glm::vec3& position)
    {
        assert(_type == Type::Point);
        _position = make_optional<glm::vec3>(position);
    }

    const glm::vec3& Light::getDirection() const 
    {
        assert(_direction.has_value());
        return *_direction;
    }

    void Light::setDirection(const glm::vec3& direction)
    {
        //TODO: Is there a way to ensure that the programmer is not 
        // calling these methods on the wrong type of light statically?
        assert(_type == Type::Directional);
        _direction = make_optional<glm::vec3>(direction);
    }
}
