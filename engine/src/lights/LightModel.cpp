#include "rev/lights/LightModels.h"

#include "rev/gl/ProgramResource.h"
#include "rev/lights/LightShaders.h"

namespace rev {

namespace {
    constexpr glm::vec2 kFullScreenQuadVertices[] = {
        { -1.0f, -1.0 },
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },

        { -1.0f, -1.0f },
        { 1.0f, 1.0f },
        { 1.0f, -1.0f },
    };
}

void PointLight::setUniforms(Camera& camera, LightProgram& program)
{
    glm::vec4 viewSpacePosition = camera.getViewMatrix() * glm::vec4(_position, 1.0f);
    program.lightPosition.set(viewSpacePosition);
    program.lightBaseColor.set(_baseColor);
    program.falloffCoefficients.set(_falloffCoefficients);
}

const glm::vec3& PointLight::getBaseColor() const { return _baseColor; }
void PointLight::setBaseColor(const glm::vec3& color) { _baseColor = color; }

const glm::vec3& PointLight::getPosition() const { return _position; }
void PointLight::setPosition(const glm::vec3& position) { _position = position; }

const glm::vec3& PointLight::getFalloffCoefficients() const { return _falloffCoefficients; }
void PointLight::setFalloffCoefficients(const glm::vec3& coefficients)
{
    _falloffCoefficients = coefficients;
}

void DirectionalLight::setUniforms(Camera& camera, LightProgram& program)
{
    glm::vec4 viewSpaceDirection = camera.getViewMatrix() * glm::vec4(_direction, 0.0f);
    program.lightDirection.set(viewSpaceDirection);
    program.lightBaseColor.set(_baseColor);
}

const glm::vec3& DirectionalLight::getBaseColor() const { return _baseColor; }
void DirectionalLight::setBaseColor(const glm::vec3& color) { _baseColor = color; }

const glm::vec3& DirectionalLight::getDirection() const { return _direction; }
void DirectionalLight::setDirection(const glm::vec3& direction) { _direction = direction; }

template <typename LightObjectType>
LightModel<LightObjectType>::LightModel(ProgramFactory& factory)
    : _program(factory.getProgram<ProgramType>())
{
    {
        VertexArrayContext context(_vao);
        context.setBuffer<GL_ARRAY_BUFFER>(_vertices);
        context.bindBufferData<GL_ARRAY_BUFFER>(gsl::span(kFullScreenQuadVertices), GL_STATIC_DRAW);

        context.setupVertexAttribute<glm::vec2>(0, 2, sizeof(glm::vec2));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
    }

    {
        auto context = _program->prepareContext();
        _program->fragPosition.set(0);
        _program->normals.set(1);

        _program->ambient.set(2);
        _program->emissive.set(3);
        _program->diffuse.set(4);
        _program->specular.set(5);
        _program->specularExponent.set(6);
    }
}

template LightModel<PointLight>::LightModel(ProgramFactory&);
template LightModel<DirectionalLight>::LightModel(ProgramFactory&);

template <typename LightObjectType>
void LightModel<LightObjectType>::render(
    Camera& camera, const std::vector<std::shared_ptr<LightObjectType>>& lights)
{
    if (lights.empty()) {
        return;
    }

    auto programContext = _program->prepareContext();
    VertexArrayContext vaoContext(_vao);
    for (const auto light : lights) {
        light->setUniforms(camera, *_program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

template void LightModel<PointLight>::render(
    Camera& camera, const std::vector<std::shared_ptr<PointLight>>& lights);
template void LightModel<DirectionalLight>::render(
    Camera& camera, const std::vector<std::shared_ptr<DirectionalLight>>& lights);

}