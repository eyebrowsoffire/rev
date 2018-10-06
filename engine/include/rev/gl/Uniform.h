#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace rev
{

template <typename VariableType>
class Uniform
{
  public:
    Uniform()
        : _location(-1)
    {
    }

    Uniform(GLint location)
        : _location(location)
    {
    }

    void set(const VariableType &value)
    {
        setUniform(_location, value);
    }

  private:
    void setUniform(GLint location, GLuint value)
    {
        glUniform1ui(location, value);
    }

    void setUniform(GLint location, GLint value)
    {
        glUniform1i(location, value);
    }

    void setUniform(GLint location, GLfloat value)
    {
        glUniform1f(location, value);
    }

    void setUniform(GLint location, const glm::vec2 &value)
    {
        glUniform2f(location, value[0], value[1]);
    }

    void setUniform(GLint location, const glm::vec3 &value)
    {
        glUniform3f(location, value[0], value[1], value[2]);
    }

    void setUniform(GLint location, const glm::vec4 &value)
    {
        glUniform4f(location, value[0], value[1], value[2], value[3]);
    }

    void setUniform(GLint location, const glm::mat2x2 &value)
    {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(GLint location, const glm::mat3x3 &value)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setUniform(GLint location, const glm::mat4x4 &value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    GLint _location;
};

} // namespace rev
