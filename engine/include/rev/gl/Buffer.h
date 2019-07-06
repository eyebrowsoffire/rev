#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

#include <gsl/span>

namespace rev {

using Buffer = Resource<singleCreate<gl::genBuffers>, singleDestroy<gl::deleteBuffers>>;

} // namespace rev
