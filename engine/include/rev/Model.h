#pragma once

#include "gl/Buffer.h"
#include "gl/VertexArray.h"

#include <glm/glm.hpp>
#include <gsl/span>

namespace rev
{

class Model
{
  public:
    Model(gsl::span<glm::vec3> vertices);

    VertexArrayContext getContext();
    size_t getVertexCount() const;

  private:
    Buffer _vertices;
    VertexArray _vao;
    size_t _vertexCount;
};

} // namespace rev