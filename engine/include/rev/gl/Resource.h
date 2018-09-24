#pragma once

#include <OpenGL/gl3.h>

namespace rev
{

template <GLuint (*createFunction)(), void (*destroyFunction)(GLuint)>
class Resource
{
  public:
    Resource()
        : _id(createFunction())
    {
    }

    ~Resource()
    {
        destroyFunction(_id);
    }

    GLuint getId() const
    {
        return _id;
    }

  private:
    GLuint _id;
};

template <void (*createFunction)(GLsizei, GLuint *)>
GLuint singleCreate()
{
    GLuint id;
    createFunction(1, &id);
    return id;
}

template <void (*destroyFunction)(GLsizei, const GLuint *)>
void singleDestroy(GLuint id)
{
    destroyFunction(1, &id);
}

} // namespace rev
