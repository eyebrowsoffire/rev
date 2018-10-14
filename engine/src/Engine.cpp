#include "rev/Engine.h"

#include "rev/Environment.h"
#include "rev/Scene.h"
#include "rev/SceneView.h"
#include "rev/Window.h"

namespace rev
{

std::shared_ptr<Scene> Engine::createScene()
{
    return std::make_shared<Scene>();
}

std::shared_ptr<SceneView> Engine::createSceneView()
{  
    return std::make_shared<SceneView>();
}

std::shared_ptr<Window> Engine::createWindow(const std::string &title,
                                             const RectSize<int> size)
{
    if (_window != nullptr)
    {
        throw std::runtime_error("Only one window per engine supported.");
    }
    _window = std::make_shared<Window>(title, size);
    return _window;
}

std::shared_ptr<Environment> Engine::createEnvironment()
{
    auto environment = std::make_shared<Environment>();
    _environments.push_back(environment);

    return environment;
}

void Engine::update()
{
    _window->makeCurrent();
    
    for (auto iter = _environments.begin(); iter != _environments.end();)
    {
        if ((*iter)->isDead())
        {
            _environments.erase(iter);
            iter = _environments.erase(iter);
        }
        else
        {
            (*iter)->tick();
            iter++;
        }
    }

    if (_window != nullptr)
    {
        _window->draw();
    }
}

} // namespace rev