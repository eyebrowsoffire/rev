#include "rev/Model.h"

#include <OpenGL/gl3.h>

namespace rev
{
Model::Model(gsl::span<const glm::vec3> vertices, gsl::span<const glm::vec3> normals)
    : _vertexCount(vertices.size())
{
    VertexArrayContext vaoContext(_vao);
    {
        ArrayBufferContext bufferContext(_vertices);

        bufferContext.bindData(vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }

    {
        ArrayBufferContext bufferContext(_normals);

        bufferContext.bindData(normals, GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }
}

VertexArrayContext Model::getContext() { return VertexArrayContext(_vao); }

size_t Model::getVertexCount() const { return _vertexCount; }
} // namespace rev
