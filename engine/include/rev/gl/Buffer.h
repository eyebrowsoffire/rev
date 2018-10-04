#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

#include <OpenGL/gl3.h>
#include <gsl/span>

namespace rev {

using Buffer = Resource<singleCreate<glGenBuffers>, singleDestroy<glDeleteBuffers>>;

template<GLenum target>
class BindBufferContext
    : public ResourceContext<Buffer, enumTargetBindFunction<glBindBuffer, target>> {
public:
    using ResourceContext<Buffer, enumTargetBindFunction<glBindBuffer, target>>::ResourceContext;

    template<typename ElementType>
    void bindData(gsl::span<const ElementType> data, GLenum usage)
    {
        glBufferData(target, data.size_bytes(), data.data(), usage);
    }
};

using ArrayBufferContext = BindBufferContext<GL_ARRAY_BUFFER>;
using ElementBufferContext = BindBufferContext<GL_ELEMENT_ARRAY_BUFFER>;

} // namespace Twitch::GraphicsUtility
