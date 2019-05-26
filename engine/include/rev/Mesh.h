#pragma once

#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"

#include <vector>

namespace rev {
class Mesh {
public:
    template <typename VertexData>
    Mesh(gsl::span<const VertexData> vertices, gsl::span<const GLuint> indices)
        : _indexCount(indices.size())
    {
        VertexArrayContext context(_vao);
        ArrayBufferContext arrayBufferContext(_vertexBuffer);
        ElementBufferContext indexBufferContext(_indexBuffer);
        arrayBufferContext.bindData(vertices, GL_STATIC_DRAW);
        indexBufferContext.bindData(indices, GL_STATIC_DRAW);

        VertexData::setupAttributes(context);
    }

    VertexArrayContext getContext()
    {
        return _vao;
    }

    void drawVertices()
    {
        ElementBufferContext context(_indexBuffer);
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
    }

private:
    Buffer _vertexBuffer;
    Buffer _indexBuffer;
    VertexArray _vao;
    size_t _indexCount;
};

template <typename VertexData>
class MeshBuilder {
public:
    template <typename... Args>
    size_t pushVertex(Args... args)
    {
        size_t index = _vertices.size();
        _vertices.emplace_back(std::forward<Args>(args)...);
        return index;
    }

    void emitTriangle(size_t firstIndex, size_t secondIndex, size_t thirdIndex)
    {
        _indices.push_back(static_cast<GLuint>(firstIndex));
        _indices.push_back(static_cast<GLuint>(secondIndex));
        _indices.push_back(static_cast<GLuint>(thirdIndex));
    }

    Mesh createMesh()
    {
        return Mesh{ gsl::span<const VertexData>(_vertices), gsl::span<const GLuint>(_indices) };
    }

private:
    std::vector<VertexData> _vertices;
    std::vector<GLuint> _indices;
};
}