#include "rev/Model.h"

#include <OpenGL/gl3.h>

namespace rev {
    Model::Model(gsl::span<glm::vec3> vertices)
    : _vertexCount(vertices.size())
    {
        VertexArrayContext vaoContext(_vao);
        ArrayBufferContext bufferContext(_vertices);

        auto data = reinterpret_cast<const std::byte*>(vertices.data());
        bufferContext.bindData({data, vertices.size()}, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }

    VertexArrayContext Model::getContext()
    {
        return VertexArrayContext(_vao);
    }

    size_t Model::getVertexCount() const
    {
        return _vertexCount;
    }
}
