#pragma once

#include "rev/gl/Context.h"
#include "rev/gl/Resource.h"

#include <glm/glm.hpp>

namespace rev {

template <typename FieldType>
struct VertexAttributeInfo;

template <>
struct VertexAttributeInfo<float> {
    static constexpr GLsizei kSize = 1;
    static constexpr GLenum kType = GL_FLOAT;
};

template <>
struct VertexAttributeInfo<glm::vec2> {
    static constexpr GLsizei kSize = 2;
    static constexpr GLenum kType = GL_FLOAT;
};

template <>
struct VertexAttributeInfo<glm::vec3> {
    static constexpr GLsizei kSize = 3;
    static constexpr GLenum kType = GL_FLOAT;
};

template <>
struct VertexAttributeInfo<glm::vec4> {
    static constexpr GLsizei kSize = 4;
    static constexpr GLenum kType = GL_FLOAT;
};

template <typename FieldType>
static constexpr GLsizei AttributeSize = VertexAttributeInfo<FieldType>::kSize;

template <typename FieldType>
static constexpr GLenum AttributeType = VertexAttributeInfo<FieldType>::kType;

using VertexArray = Resource<singleCreate<gl::genVertexArrays>,
    singleDestroy<gl::deleteVertexArrays>>;
class VertexArrayContext : public ResourceContext<VertexArray, gl::bindVertexArray> {
public:
    using ResourceContext::ResourceContext;
    
    template <typename FieldType>
    void setupVertexAttribute(GLuint attributeIndex, ptrdiff_t offset, GLsizei stride, bool normalize = false)
    {
        glEnableVertexAttribArray(attributeIndex);
        glVertexAttribPointer(attributeIndex, AttributeSize<FieldType>, AttributeType<FieldType>, (normalize ? GL_TRUE : GL_FALSE), stride,
            reinterpret_cast<void*>(offset));
    }
};

} // namespace rev
