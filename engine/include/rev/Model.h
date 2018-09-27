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
    Model(gsl::span<const glm::vec3> vertices, gsl::span<const glm::vec3> normals);

    VertexArrayContext getContext();
    size_t getVertexCount() const;

  private:
    Buffer _vertices;
    Buffer _normals;
    VertexArray _vao;
    size_t _vertexCount;
};

} // namespace rev