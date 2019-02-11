#pragma once

#include <optional>

#include <glm/glm.hpp>

namespace rev {

class Light {
public:
    enum class Type 
    {
        Directional,
        Point
    };

    Light(Type type);

    const glm::vec3& getBaseColor() const;
    void setBaseColor(const glm::vec3& baseColor);

    const glm::vec3& getDirection() const;
    void setDirection(const glm::vec3& direction);

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);

    Type getType() const;

private:
    glm::vec3 _baseColor;
    std::optional<glm::vec3> _position;
    std::optional<glm::vec3> _direction;
    Type _type;
};

} // namespace rev
