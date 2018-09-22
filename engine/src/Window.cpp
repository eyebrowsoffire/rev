#include "rev/Window.h"

#include "rev/SceneView.h"

#include <GLFW/glfw3.h>

namespace
{
void initGLFW()
{
    static int result = glfwInit();
    if (result != GLFW_TRUE)
    {
        throw std::runtime_error("Could not initialize GLFW.");
    }
}
} // namespace

namespace rev
{

struct WindowData
{
    std::shared_ptr<SceneView> sceneView;
    GLFWwindow *window;
};

Window::Window(const std::string &title, const RectSize<int> size)
    : _data(std::make_unique<WindowData>())
{
    initGLFW();

    _data->window = glfwCreateWindow(size.width, size.height, title.c_str(), NULL, NULL);
    if (_data->window == nullptr)
    {
        throw std::runtime_error("Could not create GLFW window.");
    }
}

Window::~Window()
{
}

void Window::setSceneView(std::shared_ptr<SceneView> sceneView)
{
    _data->sceneView = std::move(sceneView);
}

void Window::makeCurrent()
{
    glfwMakeContextCurrent(_data->window);
}

void Window::draw()
{
    if (_data->sceneView != nullptr)
    {
        _data->sceneView->render();
    }
    glfwSwapBuffers(_data->window);

    glfwPollEvents();
}

} // namespace rev