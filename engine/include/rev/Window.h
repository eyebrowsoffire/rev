#pragma once

#include "Types.h"
#include <memory>
#include <string>

namespace rev
{

class IKeyboardListener;
class IMouseListener;
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
  bool wantsClose();
  Point<double> getMousePosition();
  void addMouseListener(std::shared_ptr<IMouseListener> listener);
  void addKeyboardListener(std::shared_ptr<IKeyboardListener> listener);

private:
  void updateAspect();
  
  std::unique_ptr<WindowData> _data;
};

} // namespace rev