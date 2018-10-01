#pragma once

#include "Types.h"
#include <memory>
#include <string>

namespace rev
{

class SceneView;
struct WindowData;

class Window
{
public:
  ~Window();

  Window(const std::string &title, const RectSize<int> size);
  void setSceneView(std::shared_ptr<SceneView>);
  void makeCurrent();
  void draw();

private:
  void updateAspect();
  
  std::unique_ptr<WindowData> _data;
};

} // namespace rev