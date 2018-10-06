#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"
#include "rev/gl/Uniform.h"

#include <gsl/gsl_assert>
#include <string>

namespace rev {

template <GLenum shaderType> GLuint createShader() {
  return glCreateShader(shaderType);
}

namespace detail {
template <void (*propertyGetter)(GLuint, GLenum, GLint *),
          void (*logGetter)(GLuint, GLsizei, GLsizei *, GLchar *)>
std::string extractLog(GLuint objectId) {
  GLint logLength;
  propertyGetter(objectId, GL_INFO_LOG_LENGTH, &logLength);

  std::string log;
  log.resize(logLength);

  GLint fetchedLength;
  logGetter(objectId, logLength, &fetchedLength, log.data());

  log.resize(fetchedLength);

  return log;
}
} // namespace detail

template <GLenum shaderType>
class Shader : public Resource<createShader<shaderType>, glDeleteShader> {
public:
  void setSource(std::string_view source) {
    auto *data = source.data();
    auto length = source.size();
    Expects(length <= std::numeric_limits<GLint>::max());

    auto glLength = static_cast<GLint>(length);
    glShaderSource(this->getId(), 1, &data, &glLength);
  }

  void compile() { glCompileShader(this->getId()); }

  bool getCompileStatus() {
    GLint status;
    glGetShaderiv(this->getId(), GL_COMPILE_STATUS, &status);
    return (status == GL_TRUE);
  }

  std::string getCompileLog() {
    return detail::extractLog<glGetShaderiv, glGetShaderInfoLog>(this->getId());
  }
};

using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

class Program : public Resource<glCreateProgram, glDeleteProgram> {
public:
  template <GLenum shaderType>
  void attachShader(const Shader<shaderType> &shader) {
    glAttachShader(getId(), shader.getId());
  }

  void link() { glLinkProgram(getId()); }

  bool getLinkStatus() {
    GLint status;
    glGetProgramiv(getId(), GL_LINK_STATUS, &status);
    return (status == GL_TRUE);
  }

  std::string getLinkLog() {
    return detail::extractLog<glGetProgramiv, glGetProgramInfoLog>(getId());
  }

  void buildWithSource(std::string_view vertexSource,
                       std::string_view fragmentSource) {
    VertexShader vShader;
    vShader.setSource(vertexSource);
    vShader.compile();
    if (!vShader.getCompileStatus()) {
      throw vShader.getCompileLog();
    }

    FragmentShader fShader;
    fShader.setSource(fragmentSource);
    fShader.compile();
    if (!fShader.getCompileStatus()) {
      throw fShader.getCompileLog();
    }

    attachShader(vShader);
    attachShader(fShader);
    link();
    if (!getLinkStatus()) {
      throw getLinkLog();
    }
  }

  template <typename VariableType>
  Uniform<VariableType> getUniform(const char *name) {
    GLint location = glGetUniformLocation(getId(), name);

    return Uniform<VariableType>(location);
  }
};

using ProgramContext = ResourceContext<Program, glUseProgram>;

} // namespace rev
