#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

namespace rev {

using Texture =
    Resource<singleCreate<glGenTextures>, singleDestroy<glDeleteTextures>>;

template <GLenum target>
class TextureContext
    : public ResourceContext<Texture,
                             enumTargetBindFunction<glBindTexture, target>> {
public:
  using ResourceContext<
      Texture, enumTargetBindFunction<glBindTexture, target>>::ResourceContext;

  void setParameter(GLenum name, GLint value) {
    glTexParameteri(target, name, value);
  }

  void setImage(GLint level, GLenum internalFormat, GLsizei width,
                GLsizei height, GLint border, GLenum format, GLenum type,
                const void *pixels) {
    glTexImage2D(target, level, internalFormat, width, height, border, format,
                 type, pixels);
  }
};

using Texture2DContext = TextureContext<GL_TEXTURE_2D>;

} // namespace rev
