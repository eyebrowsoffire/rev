#pragma once

#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"
#include "rev/IModel.h"

#include <glm/glm.hpp>
#include <gsl/span>

namespace rev
{

class BasicModel : public IModel
{
  public:
    BasicModel(gsl::span<const glm::vec3> vertices, gsl::span<const glm::vec3> normals);

    VertexArrayContext getVertexArrayContext() override;
    void draw() override;

  private:
    Buffer _vertices;
    Buffer _normals;

    VertexArray _vao;
    size_t _vertexCount;
};

} // namespace rev