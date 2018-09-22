#include <rev/Engine.h>
#include <rev/Window.h>

int main(void) {
  rev::Engine engine;
  auto window = engine.createWindow("Test Game", {640, 480});
  window->setSceneView(engine.createSceneView());
  while(1)
  {
    engine.update();
  }
  return 0;
}
