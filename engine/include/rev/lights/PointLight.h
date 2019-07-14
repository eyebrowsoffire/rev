#pragma once

#include "rev/Camera.h"
#include "rev/ProgramFactory.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace rev {

class PointLightProgram;

class PointLight {
public:
    const glm::vec3& getBaseColor() const;
    void setBaseColor(const glm::vec3& color);

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);

    // Gets/sets the coefficients for the quadratic falloff attenuation function.
    // The nth element of the vector contains the coefficient for the nth power term.
    const glm::vec3& getFalloffCoefficients() const;
    void setFalloffCoefficients(const glm::vec3& coefficients);

private:
    glm::vec3 _baseColor{ 1.0f, 1.0f, 1.0f };
    glm::vec3 _position{};
    glm::vec3 _falloffCoefficients{ 1.0f, 0.0f, 0.1f };
};

class PointLightModel {
public:
    using SceneObjectType = PointLight;
    PointLightModel(ProgramFactory& factory);

    void render(Camera& camera, const std::vector<std::shared_ptr<SceneObjectType>>& objects);

private:
    std::shared_ptr<PointLightProgram> _program;
    VertexArray _vao;
    Buffer _vertices;
};

} // namespace rev