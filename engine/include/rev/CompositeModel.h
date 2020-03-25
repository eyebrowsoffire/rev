#pragma once

#include "rev/Camera.h"
#include "rev/DrawMaterialsProgram.h"
#include "rev/MaterialProperties.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"
#include "rev/shaders/ShaderLibrary.h"

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

namespace rev {

struct CompositeObject {
    glm::mat4 transform{ 1.0f };
};

struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
};

class ModelComponent {
public:
    ModelComponent(GLsizei indexCount, size_t indexOffset, const MaterialProperties& properties)
        : _properties(properties)
        , _indexCount(indexCount)
        , _indexOffset(indexOffset)
    {
    }

    void draw()
    {
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT,
            reinterpret_cast<void*>(_indexOffset * sizeof(GLuint)));
    }

    const MaterialProperties& getMaterialProperties() const { return _properties; }

private:
    size_t _indexOffset;
    GLsizei _indexCount;
    MaterialProperties _properties;
};

class CompositeModel {
public:
    using SceneObjectType = CompositeObject;

    CompositeModel(ShaderLibrary& library, std::vector<ModelComponent>&& components,
        gsl::span<const VertexData> vertices, gsl::span<const GLuint> indices)
        : _components(std::move(components))
        , _program(library.acquireProgram(
              std::array{ VertexShaderComponentInfo::make<DrawMaterialsVertexComponent>() },
              std::array{ FragmentShaderComponentInfo::make<DrawMaterialsFragmentComponent>() }))
        , _model(_program->getUniform<glm::mat4>("model"))
        , _view(_program->getUniform<glm::mat4>("view"))
        , _projection(_program->getUniform<glm::mat4>("projection"))
        , _ambient(_program->getUniform<glm::vec3>("fAmbient"))
        , _emissive(_program->getUniform<glm::vec3>("fEmissive"))
        , _diffuse(_program->getUniform<glm::vec3>("fDiffuse"))
        , _specular(_program->getUniform<glm::vec3>("fSpecular"))
        , _specularExponent(_program->getUniform<float>("fSpecularExponent"))
    {
        auto component = VertexShaderComponentInfo::make<DrawMaterialsVertexComponent>();

        VertexArrayContext context(_vao);

        context.setBuffer<GL_ARRAY_BUFFER>(_vertices);
        context.bindBufferData<GL_ARRAY_BUFFER>(vertices, GL_STATIC_DRAW);

        context.setBuffer<GL_ELEMENT_ARRAY_BUFFER>(_indices);
        context.bindBufferData<GL_ELEMENT_ARRAY_BUFFER>(indices, GL_STATIC_DRAW);

        context.setupVertexAttribute<decltype(VertexData::position)>(
            0, offsetof(VertexData, position), sizeof(VertexData));
        context.setupVertexAttribute<decltype(VertexData::normal)>(
            1, offsetof(VertexData, normal), sizeof(VertexData));
    }

    void render(Camera& camera, gsl::span<std::shared_ptr<CompositeObject>> objects)
    {
        auto programContext = _program->prepareContext();
        VertexArrayContext vertexContext(_vao);

        _view.set(camera.getViewMatrix());
        _projection.set(camera.getProjectionMatrix());

        for (auto& object : objects) {
            _model.set(object->transform);

            for (auto& component : _components) {
                bindMaterialProperties(component.getMaterialProperties());
                component.draw();
            }
        }
    }

private:
    void bindMaterialProperties(const MaterialProperties& properties)
    {
        _ambient.set(properties.ambientColor);
        _emissive.set(properties.emissiveColor);
        _diffuse.set(properties.diffuseColor);
        _specular.set(properties.specularColor);
        _specularExponent.set(properties.specularExponent);
    }

    std::shared_ptr<Program> _program;

    Uniform<glm::mat4> _model;
    Uniform<glm::mat4> _view;
    Uniform<glm::mat4> _projection;

    Uniform<glm::vec3> _ambient;
    Uniform<glm::vec3> _emissive;
    Uniform<glm::vec3> _diffuse;
    Uniform<glm::vec3> _specular;
    Uniform<float> _specularExponent;

    VertexArray _vao;
    Buffer _vertices;
    Buffer _indices;
    std::vector<ModelComponent> _components;
};
} // namespace rev