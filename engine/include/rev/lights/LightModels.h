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
class DirectionalLightProgram;
class SpotLightProgram;

class PointLight {
public:
    using LightProgram = PointLightProgram;

    void setUniforms(Camera& camera, LightProgram& program);

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

class DirectionalLight {
public:
    using LightProgram = DirectionalLightProgram;

    void setUniforms(Camera& camera, LightProgram& program);

    const glm::vec3& getBaseColor() const;
    void setBaseColor(const glm::vec3& color);

    const glm::vec3& getDirection() const;
    void setDirection(const glm::vec3& direction);

private:
    glm::vec3 _baseColor{ 1.0f, 1.0f, 1.0f };
    glm::vec3 _direction{ 0.0f, -1.0f, 0.0f };
};

class SpotLight {
public:
    using LightProgram = SpotLightProgram;

    void setUniforms(Camera& camera, SpotLightProgram& program);

    const glm::vec3& getBaseColor() const;
    void setBaseColor(const glm::vec3& color);

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& position);

    const glm::vec3& getDirection() const;
    void setDirection(const glm::vec3& direction);

    const float getConeAngle() const;
    void setConeAngle(float coneAngle);

    const float getSoftAngleThreshold() const;
    void setSoftAngleThreshold(float threshold);

    // Gets/sets the coefficients for the quadratic falloff attenuation function.
    // The nth element of the vector contains the coefficient for the nth power term.
    const glm::vec3& getFalloffCoefficients() const;
    void setFalloffCoefficients(const glm::vec3& coefficients);

private:
    glm::vec3 _baseColor{ 1.0f, 1.0f, 1.0f };
    glm::vec3 _position{};
    glm::vec3 _direction{ 0.0f, -1.0f, 0.0f };
    glm::vec3 _falloffCoefficients{ 1.0f, 0.0f, 0.1f };
    float _coneAngle = 0.5f;
    float _softAngleThreshold = 0.0f;
};

template <typename LightObjectType>
class LightModel {
public:
    using SceneObjectType = LightObjectType;
    using ProgramType = typename LightObjectType::LightProgram;

    LightModel(ProgramFactory& factory);

    void render(Camera& camera, const std::vector<std::shared_ptr<SceneObjectType>>& objects);

private:
    std::shared_ptr<ProgramType> _program;
    VertexArray _vao;
    Buffer _vertices;
};

using PointLightModel = LightModel<PointLight>;
using DirectionalLightModel = LightModel<DirectionalLight>;
using SpotLightModel = LightModel<SpotLight>;

} // namespace rev