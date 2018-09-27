#pragma once

#include "rev/Camera.h"
#include "rev/gl/Program.h"
#include "rev/gl/Uniform.h"

#include <glm/glm.hpp>
#include <memory>

namespace rev
{
class Scene;

class SceneView
{
  public:
    SceneView();
    void setScene(std::shared_ptr<Scene>);
    void render();

    const std::shared_ptr<Camera>& getCamera() const;

  private:
    std::shared_ptr<Scene> _scene;
    std::shared_ptr<Camera> _camera;
    Program _program;
    Uniform<glm::mat4> _model;
    Uniform<glm::mat4> _view;
    Uniform<glm::mat4> _projection;
};
} // namespace rev