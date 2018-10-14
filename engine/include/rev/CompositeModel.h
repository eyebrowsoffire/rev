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

namespace rev
{

struct CompositeObject
{
    CompositeObject()
    : transform(1.0f)
    {
    }
    glm::mat4 transform;
};

struct VertexData
{
    glm::vec3 position;
    glm::vec3 normal;
};

class ModelComponent
{
  public:
    ModelComponent(gsl::span<const GLuint> indexes,
                   const MaterialProperties &properties)
        : _properties(properties), _indexCount(indexes.size())
    {
        ElementBufferContext context(_indexes);
        context.bindData(indexes, GL_STATIC_DRAW);
    }

    void draw(DrawMaterialsProgram &program)
    {
        program.ambient.set(_properties.ambientColor);
        program.emissive.set(_properties.emissiveColor);
        program.diffuse.set(_properties.diffuseColor);
        program.specular.set(_properties.specularColor);
        program.specularExponent.set(_properties.specularExponent);

        ElementBufferContext context(_indexes);
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
    }

  private:
    Buffer _indexes;
    GLsizei _indexCount;
    MaterialProperties _properties;
};

class CompositeModel
{
  public:
    using SceneObjectType = CompositeObject;

    CompositeModel(ProgramFactory &factory,
                   std::vector<ModelComponent> &&components,
                   gsl::span<const VertexData> vertices)
        : _components(std::move(components)),
          _program(factory.getProgram<DrawMaterialsProgram>())
    {
        VertexArrayContext vertexContext(_vao);

        ArrayBufferContext bufferContext(_vertices);

        bufferContext.bindData(vertices, GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                              nullptr);

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                              reinterpret_cast<void *>(sizeof(glm::vec3)));
    }

    void render(Camera &camera, gsl::span<std::shared_ptr<CompositeObject>> objects)
    {
        auto programContext = _program->prepareContext();
        VertexArrayContext vertexContext(_vao);

        _program->view.set(camera.getViewMatrix());
        _program->projection.set(camera.getProjectionMatrix());

        for (auto &object : objects)
        {
            _program->model.set(object->transform);

            for (auto &component : _components)
            {
                component.draw(*_program);
            }
        }
    }

  private:
    std::shared_ptr<DrawMaterialsProgram> _program;
    VertexArray _vao;
    Buffer _vertices;
    std::vector<ModelComponent> _components;
};
} // namespace rev