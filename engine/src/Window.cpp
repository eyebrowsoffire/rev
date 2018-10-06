#include "rev/Window.h"

#include "rev/gl/OpenGL.h"
#include "rev/SceneView.h"

#include <iostream>

namespace rev
{

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

constexpr const char *kVertexShader = R"vertexShader(
        #version 330 core

        layout(location = 0) in vec2 vPosition;

        uniform vec2 aspect;

        out vec2 fPosition;

        void main()
        {
            vec2 position = vPosition * aspect;
            gl_Position = vec4(position, 0.0f, 1.0f);
            fPosition = (vPosition + 1.0f) / 2.0f;
        }
    )vertexShader";

constexpr const char *kFragmentShader = R"fragmentShader(
        #version 330 core

        in vec2 fPosition;

        uniform sampler2D inputTexture;

        out vec4 fragColor;

        void main() 
        {
            fragColor = texture(inputTexture, fPosition);
        }
    )fragmentShader";

constexpr glm::vec2 kFullScreenQuadVertices[] = {
    {-1.0f, -1.0},  {-1.0f, 1.0f}, {1.0f, 1.0f},

    {-1.0f, -1.0f}, {1.0f, 1.0f},  {1.0f, -1.0f}};

} // namespace

struct WindowData
{
    std::shared_ptr<SceneView> sceneView;
    GLFWwindow *window;
    Program drawProgram;
    glm::vec2 aspect;
    Uniform<glm::vec2> aspectUniform;
    VertexArray vao;
    Buffer vertexData;
};

Window::Window(const std::string &title, const RectSize<int> size)
{
    initGLFW();
    glfwSetErrorCallback([](int errorCode, const char* message) {
        std::cout << "Error Code: " << errorCode << std::endl;
        std::cout << "Message: " << message << std::endl;
    });
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    
    GLFWwindow *window = glfwCreateWindow(size.width, size.height, title.c_str(), NULL, NULL);
    if (window == nullptr)
    {
        throw std::runtime_error("Could not create GLFW window.");
    }

    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);

    _data = std::make_unique<WindowData>();
    _data->window = window;
    
    _data->drawProgram.buildWithSource(kVertexShader, kFragmentShader);
    _data->aspectUniform = _data->drawProgram.getUniform<glm::vec2>("aspect");
    _data->drawProgram.getUniform<GLint>("inputTexture").set(0);

    glfwSetFramebufferSizeCallback(_data->window, [](GLFWwindow* window, int, int) {
        Window* myThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myThis->updateAspect();
    });
    updateAspect();

    VertexArrayContext vaoContext(_data->vao);
    ArrayBufferContext bufferContext(_data->vertexData);

    bufferContext.bindData(gsl::span<const glm::vec2>(kFullScreenQuadVertices),
                           GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
}

Window::~Window()
{
}

void Window::setSceneView(std::shared_ptr<SceneView> sceneView)
{
    _data->sceneView = std::move(sceneView);
    updateAspect();
}

void Window::makeCurrent()
{
    glfwMakeContextCurrent(_data->window);
}

void Window::draw()
{
    if (_data->sceneView == nullptr)
    {
        return;
    }
    _data->sceneView->render();

    int width;
    int height;
    glfwGetFramebufferSize(_data->window, &width, &height);

    {
        ProgramContext progContext(_data->drawProgram);
        VertexArrayContext vaoContext(_data->vao);
        glViewport(0, 0, width, height);

        glEnable(GL_FRAMEBUFFER_SRGB);

        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _data->sceneView->getOutputTexture().getId());

        glDisable(GL_BLEND);

        _data->aspectUniform.set(_data->aspect);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(_data->window);

    glfwPollEvents();
}

void Window::updateAspect()
{
    if(_data->sceneView == nullptr)
    {
        return;
    }

    RectSize<GLsizei> contentSize = _data->sceneView->getOutputSize();
    if (contentSize.height == 0 || contentSize.width == 0)
    {
        return;
    }

    RectSize<GLsizei> frameBufferSize;
    glfwGetFramebufferSize(_data->window, &frameBufferSize.width, &frameBufferSize.height);

    float contentAspect = static_cast<float>(contentSize.width) / static_cast<float>(contentSize.height);
    float frameBufferAspect = static_cast<float>(frameBufferSize.width) / static_cast<float>(frameBufferSize.height);

    if (frameBufferAspect > contentAspect)
    {
        _data->aspect = {(contentAspect / frameBufferAspect), 1.0f};
    }
    else
    {
        _data->aspect = {1.0f, (frameBufferAspect / contentAspect)};
    }
}

} // namespace rev