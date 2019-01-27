#pragma once

#include <glm/glm.hpp>

namespace rev {

class Light {
protected:
    Light();
public:
    const glm::vec3& getBaseColor() const;
    void setBaseColor(const glm::vec3& color);

    const glm::mat4x4& getLightSpaceTransform() const;
    void setLightSpaceTransform(const glm::mat4x4& transform);
private:
    glm::vec3 _baseColor;
};

class PointLight: public Light {
public:
    PointLight();

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);
private:
    glm::vec3 _position;
};

class DirectionalLight: public Light {
public:
    DirectionalLight();

    const glm::vec3& getDirection() const;
    void setDirection(const glm::vec3& direction);
private:
    glm::vec3 _direction;
};

} // namespace rev