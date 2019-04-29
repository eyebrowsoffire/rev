#pragma once

#include "rev/Camera.h"
#include "rev/ProgramFactory.h"

namespace rev {

// A type-erased object group.
// This is the interface used by the rendering engine to render all objects in
// the group.
class ISceneObjectGroup {
public:
  ~ISceneObjectGroup() = default;
  virtual void render(Camera &camera) = 0;
};

// Models conform to the following contract:
// class Model
// {
//   public:
//     // Models must specify an object type that represents a single instance
//     of this model. using SceneObjectType;
//
//     // Renders the group of objects from the viewpoint of the passed in
//     camera. void render(Camera& camera, const
//     std::vector<std::shared_ptr<SceneObjectType>>& objects);
// };

// Encapsulates a group of objects that all render with the same model.
template <typename ModelType>
class SceneObjectGroup : public ISceneObjectGroup {
public:
  using SceneObjectType = typename ModelType::SceneObjectType;

  // SceneObjectGroup is constructed from the arguments for its model. The model
  // is created on construction from the arguments.
  template <typename... Args>
  SceneObjectGroup(Args &&... args) : _model(std::forward<Args>(args)...) {}

  // Renders all the objects in the group. Used by the rendering engine.
  void render(Camera &camera) override { _model.render(camera, _objects); }

  // Creates a new object, adds it to the group, and returns it.
  // TODO: Maybe this should take arguments, that it should forward to the
  // constructor of the SceneObjectType?
  std::shared_ptr<SceneObjectType> addObject() {
    auto object = std::make_shared<SceneObjectType>();
    _objects.push_back(object);

    return std::move(object);
  }

  // TODO: write removeObject();

private:
  ModelType _model;
  std::vector<std::shared_ptr<SceneObjectType>> _objects;
};
} // namespace rev
