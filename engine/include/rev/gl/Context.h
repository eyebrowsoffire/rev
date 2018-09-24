#pragma once

#include <OpenGL/gl3.h>

namespace rev {

template<typename ResourceType, void (*bindFunction)(GLuint)>
class ResourceContext {
public:
    ResourceContext(GLuint resourceId)
        : _resourceId(resourceId)
        , _previousContext(sCurrentContext)
    {
        sCurrentContext = this;
        bindFunction(_resourceId);
    }

    ResourceContext(const ResourceType &resource)
        : ResourceContext(resource.getId())
    {
    }

    ~ResourceContext()
    {
        GLuint previousResourceId =
            (_previousContext == nullptr) ? 0 : _previousContext->getResourceId();
        bindFunction(previousResourceId);
        sCurrentContext = _previousContext;
    }

    GLuint getResourceId() const
    {
        return _resourceId;
    }

private:
    static thread_local inline ResourceContext *sCurrentContext = nullptr;

    ResourceContext *_previousContext;
    GLuint _resourceId;
};

template<void (*bindFunction)(GLenum, GLuint), GLenum enumValue>
void enumTargetBindFunction(GLuint resource)
{
    bindFunction(enumValue, resource);
}

} // namespace Twitch::GraphicsUtility
