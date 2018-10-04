#pragma once

#include "rev/IModel.h"
#include "rev/gl/VertexArray.h"
#include "rev/gl/Buffer.h"

#include <glm/glm.hpp>
#include <gsl/span>
#include <string_view>

namespace rev {

struct VertexData {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinate;
};

class IndexedModel : public IModel {
public:
    IndexedModel(gsl::span<const VertexData> vertexData, gsl::span<const GLuint> vertexIndexes);

    VertexArrayContext getVertexArrayContext() override;
    void draw() override;
private:
    VertexArray _vao;
    Buffer _vertexAttributes;
    Buffer _elementIndexes;
    size_t _indexCount;
};

} // namespace rev