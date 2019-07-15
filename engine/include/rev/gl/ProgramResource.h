#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"
#include "rev/gl/Uniform.h"

#include <array>
#include <gsl/gsl_assert>
#include <string>

namespace rev {

template <GLenum shaderType>
GLuint createShader()
{
    return glCreateShader(shaderType);
}

namespace detail {
    template <void (*propertyGetter)(GLuint, GLenum, GLint*),
        void (*logGetter)(GLuint, GLsizei, GLsizei*, GLchar*)>
    std::string extractLog(GLuint objectId)
    {
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
class Shader : public Resource<createShader<shaderType>, gl::deleteShader> {
public:
    void setSource(const std::string_view& source)
    {
        setSource(std::array<std::string_view, 1>{ source });
    }

    template <size_t arrayLength>
    void setSource(const std::array<std::string_view, arrayLength>& source)
    {
        std::array<const char*, arrayLength> pointers;
        std::array<GLint, arrayLength> lengths;
        for (size_t i = 0; i < arrayLength; i++) {
            pointers[i] = source[i].data();
            lengths[i] = static_cast<GLint>(source[i].size());
            Expects(source[i].size() <= std::numeric_limits<GLint>::max());
        }
        glShaderSource(this->getId(), arrayLength, pointers.data(), lengths.data());
    }

    void compile() { glCompileShader(this->getId()); }

    bool getCompileStatus()
    {
        GLint status;
        glGetShaderiv(this->getId(), GL_COMPILE_STATUS, &status);
        return (status == GL_TRUE);
    }

    std::string getCompileLog()
    {
        return detail::extractLog<gl::getShaderiv, gl::getShaderInfoLog>(this->getId());
    }
};

using VertexShader = Shader<GL_VERTEX_SHADER>;
using FragmentShader = Shader<GL_FRAGMENT_SHADER>;

class ProgramResource : public Resource<gl::createProgram, gl::deleteProgram> {
public:
    template <GLenum shaderType>
    void attachShader(const Shader<shaderType>& shader)
    {
        glAttachShader(getId(), shader.getId());
    }

    void link() { glLinkProgram(getId()); }

    bool getLinkStatus()
    {
        GLint status;
        glGetProgramiv(getId(), GL_LINK_STATUS, &status);
        return (status == GL_TRUE);
    }

    std::string getLinkLog()
    {
        return detail::extractLog<gl::getProgramiv, gl::getProgramInfoLog>(getId());
    }

    template <typename VertexSourceType, typename FragmentSourceType>
    void buildWithSource(
        const VertexSourceType& vertexSource, const FragmentSourceType& fragmentSource)
    {
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
    Uniform<VariableType> getUniform(const char* name)
    {
        GLint location = glGetUniformLocation(getId(), name);

        return Uniform<VariableType>(location);
    }
};

using ProgramContext = ResourceContext<ProgramResource, gl::useProgram>;

} // namespace rev
