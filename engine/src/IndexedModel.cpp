#include "rev/IndexedModel.h"

#include <fstream>

namespace rev {
    IndexedModel::IndexedModel(gsl::span<const VertexData> vertexData, gsl::span<const GLuint> vertexIndexes)
    : _indexCount(vertexIndexes.size())
    {
        VertexArrayContext vaoContext(_vao);
        {
            ArrayBufferContext bufferContext(_vertexAttributes);

            bufferContext.bindData(vertexData, GL_STATIC_DRAW);

            // Position
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), nullptr);

            // Normal
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(sizeof(glm::vec3)));

            // Texture coordinate
            // TODO: fill this in.
        }

        {
            ElementBufferContext bufferContext(_elementIndexes);
            bufferContext.bindData(vertexIndexes, GL_STATIC_DRAW);
        }
    }

    VertexArrayContext IndexedModel::getVertexArrayContext()
    {
        return VertexArrayContext(_vao);
    }

    void IndexedModel::draw()
    {
        ElementBufferContext bufferContext(_elementIndexes);
        glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr);
    }
}
