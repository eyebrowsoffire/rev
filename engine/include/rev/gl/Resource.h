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

    Resource(const Resource&) = delete;
    Resource(Resource&& src)
    : _id(src._id)
    {
        src._id = 0;
    }

    Resource& operator=(const Resource&) = delete;
    Resource& operator=(Resource&& src) = delete;

    ~Resource()
    {
        if(_id == 0)
        {
            return;
        }
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
