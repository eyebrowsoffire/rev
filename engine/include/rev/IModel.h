#pragma once

#include "gl/VertexArray.h"

namespace rev {

class IModel {
public:
  virtual ~IModel() = default;
  virtual VertexArrayContext getVertexArrayContext() = 0;
  virtual void draw() = 0;
};

} // namespace rev