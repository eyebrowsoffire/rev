#pragma once

#include "Types.h"

#include <memory>
#include <vector>
#include <string>

namespace rev
{
class Environment;
class Scene;
class SceneView;
class Window;

class Engine
{
public:
  std::shared_ptr<Scene> createScene();
  std::shared_ptr<SceneView> createSceneView();
  std::shared_ptr<Window> createWindow(const std::string &title, const RectSize<int> size);
  std::shared_ptr<Environment> createEnvironment();

  void update();

private:
  std::shared_ptr<Window> _window;
  std::vector<std::shared_ptr<Environment>> _environments;
};

} // namespace rev