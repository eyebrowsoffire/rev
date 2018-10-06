#include "rev/BasicModel.h"

#include "rev/gl/OpenGL.h"

namespace rev
{
BasicModel::BasicModel(gsl::span<const glm::vec3> vertices, gsl::span<const glm::vec3> normals)
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

VertexArrayContext BasicModel::getVertexArrayContext() { return VertexArrayContext(_vao); }

void BasicModel::draw()
{
    glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
}
} // namespace rev
