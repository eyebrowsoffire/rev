#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

#include <gsl/span>

namespace rev {

using Buffer = Resource<singleCreate<gl::genBuffers>, singleDestroy<gl::deleteBuffers>>;

template <GLenum target>
class BindBufferContext
    : public ResourceContext<Buffer,
          enumTargetBindFunction<gl::bindBuffer, target>> {
public:
    using ResourceContext<
        Buffer, enumTargetBindFunction<gl::bindBuffer, target>>::ResourceContext;

    template <typename ElementType, std::ptrdiff_t extent>
    void bindData(gsl::span<const ElementType, extent> data, GLenum usage)
    {
        glBufferData(target, data.size_bytes(), data.data(), usage);
    }
};

using ArrayBufferContext = BindBufferContext<GL_ARRAY_BUFFER>;
using ElementBufferContext = BindBufferContext<GL_ELEMENT_ARRAY_BUFFER>;

} // namespace rev
