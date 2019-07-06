#pragma once

#include "rev/Camera.h"
#include "rev/DrawMaterialsProgram.h"
#include "rev/MaterialProperties.h"
#include "rev/ProgramFactory.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"

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

    void draw(DrawMaterialsProgram& program)
    {
        program.ambient.set(_properties.ambientColor);
        program.emissive.set(_properties.emissiveColor);
        program.diffuse.set(_properties.diffuseColor);
        program.specular.set(_properties.specularColor);
        program.specularExponent.set(_properties.specularExponent);

        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT,
            reinterpret_cast<void*>(_indexOffset * sizeof(GLuint)));
    }

private:
    size_t _indexOffset;
    GLsizei _indexCount;
    MaterialProperties _properties;
};

class CompositeModel {
public:
    using SceneObjectType = CompositeObject;

    CompositeModel(ProgramFactory& factory, std::vector<ModelComponent>&& components,
        gsl::span<const VertexData> vertices, gsl::span<const GLuint> indices)
        : _components(std::move(components))
        , _program(factory.getProgram<DrawMaterialsProgram>())
    {
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

        _program->view.set(camera.getViewMatrix());
        _program->projection.set(camera.getProjectionMatrix());

        for (auto& object : objects) {
            _program->model.set(object->transform);

            for (auto& component : _components) {
                component.draw(*_program);
            }
        }
    }

private:
    std::shared_ptr<DrawMaterialsProgram> _program;
    VertexArray _vao;
    Buffer _vertices;
    Buffer _indices;
    std::vector<ModelComponent> _components;
};
} // namespace rev