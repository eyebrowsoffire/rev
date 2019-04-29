#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"
#include "rev/gl/Texture.h"

#include <string>

namespace rev {

using FrameBuffer = Resource<singleCreate<glGenFramebuffers>,
                             singleDestroy<glDeleteFramebuffers>>;

template <GLenum Target>
class FrameBufferContext
    : public ResourceContext<
          FrameBuffer, enumTargetBindFunction<glBindFramebuffer, Target>> {
public:
  using ResourceContext<
      FrameBuffer,
      enumTargetBindFunction<glBindFramebuffer, Target>>::ResourceContext;

  void setTextureAttachment(GLenum attachment, const Texture &texture) {
    glFramebufferTexture2D(Target, attachment, GL_TEXTURE_2D, texture.getId(),
                           0);
  }

  GLint getAttachmentParameter(GLenum parameterName, GLenum attachment) {
    GLint result;
    glGetFramebufferAttachmentParameteriv(Target, attachment, parameterName,
                                          &result);
    return result;
  }
};

using ReadFrameBufferContext = FrameBufferContext<GL_READ_FRAMEBUFFER>;
using WriteFrameBufferContext = FrameBufferContext<GL_DRAW_FRAMEBUFFER>;
using ReadWriteFrameBufferContext = FrameBufferContext<GL_FRAMEBUFFER>;

} // namespace rev
