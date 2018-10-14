#pragma once

#include "rev/Camera.h"
#include "rev/ProgramFactory.h"

namespace rev
{
class ISceneObjectGroup
{
public:
    ~ISceneObjectGroup() = default;
    virtual void render(Camera &camera) = 0;
};

template <typename ModelType>
class SceneObjectGroup : public ISceneObjectGroup
{
public:
    using SceneObjectType = typename ModelType::SceneObjectType;

    template <typename... Args>
    SceneObjectGroup(Args&& ...args)
    : _model(std::forward<Args>(args)...)
    {
    }

    void render(Camera &camera) override
    {
        _model.render(camera, _objects);
    }

    std::shared_ptr<SceneObjectType> addObject()
    {
        auto object = std::make_shared<SceneObjectType>();
        _objects.push_back(object);

        return std::move(object);
    }

private:
    ModelType _model;
    std::vector<std::shared_ptr<SceneObjectType>> _objects;
};
}
