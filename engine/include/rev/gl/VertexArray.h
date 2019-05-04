#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

namespace rev {

using VertexArray = Resource<singleCreate<glGenVertexArrays>,
    singleDestroy<glDeleteVertexArrays>>;
using VertexArrayContext = ResourceContext<VertexArray, glBindVertexArray>;

} // namespace rev
