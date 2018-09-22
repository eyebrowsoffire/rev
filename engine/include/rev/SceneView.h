#pragma once

#include <memory>

namespace rev
{
class Scene;

class SceneView
{
public:
    void setScene(std::shared_ptr<Scene>);
    void render();
private:
    std::shared_ptr<Scene> _scene;
};
}