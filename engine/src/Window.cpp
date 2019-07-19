#include "rev/Window.h"

#include "rev/IKeyboardListener.h"
#include "rev/IMouseListener.h"
#include "rev/SceneView.h"
#include "rev/gl/OpenGL.h"

#include <iostream>
#include <memory>
#include <vector>

namespace rev {

namespace {
    constexpr const char* kSwitchMapping
        = "030000007e0500000920000000020000,Nintendo Switch Pro "
          "Controller,a:b0,b:b1,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,"
          "leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,"
          "rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b2,y:b3,";

    void initGLFW()
    {
        static int result = glfwInit();
        if (result != GLFW_TRUE) {
            throw std::runtime_error("Could not initialize GLFW.");
        }
    }

    constexpr const char* kVertexShader = R"vertexShader(
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

    constexpr const char* kFragmentShader = R"fragmentShader(
        #version 330 core

        in vec2 fPosition;

        uniform sampler2D inputTexture;

        out vec4 fragColor;

        void main() 
        {
            fragColor = texture(inputTexture, fPosition);
        }
    )fragmentShader";

    constexpr glm::vec2 kFullScreenQuadVertices[]
        = { { -1.0f, -1.0 }, { -1.0f, 1.0f }, { 1.0f, 1.0f },

              { -1.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, -1.0f } };

    void dumpJoysticks()
    {
        glfwUpdateGamepadMappings(kSwitchMapping);

        for (int id = GLFW_JOYSTICK_1; id <= GLFW_JOYSTICK_LAST; id++) {
            std::cout << "GLFW_JOYSTICK_" << (id + 1) << ": ";
            if (!glfwJoystickPresent(id)) {
                std::cout << "GLFW_JOYSTICK_" << (id + 1) << "Unplugged" << std::endl;
                continue;
            }
            std::cout << glfwGetJoystickName(id) << std::endl;
            bool isGamePad = glfwJoystickIsGamepad(id);
            std::cout << "  Gamepad: " << (isGamePad ? "yes" : "no") << std::endl;
            int axisCount;
            const float* axisValues = glfwGetJoystickAxes(id, &axisCount);
            if (!axisCount) {
                std::cout << "  No axes." << std::endl;
            } else {
                std::cout << "  Axes: (" << axisCount << ")[" << std::endl;
                for (int i = 0; i < axisCount; i++) {
                    std::cout << "    " << axisValues[i] << std::endl;
                }
                std::cout << "  ]" << std::endl;
            }

            int buttonCount;
            const unsigned char* buttonStates = glfwGetJoystickButtons(id, &buttonCount);
            if (!buttonCount) {
                std::cout << "  No buttons." << std::endl;
            } else {
                std::cout << "  Buttons: (" << buttonCount << ")[" << std::endl;
                for (int i = 0; i < buttonCount; i++) {
                    std::cout << "    " << (buttonStates[i] ? "on" : "off") << std::endl;
                }
                std::cout << "  ]" << std::endl;
            }

            int hatCount;
            const unsigned char* hatStates = glfwGetJoystickHats(id, &hatCount);
            if (!hatCount) {
                std::cout << "  No hats." << std::endl;
            } else {
                std::cout << "  Hats: (" << hatCount << ")[" << std::endl;
                for (int i = 0; i < hatCount; i++) {
                    std::cout << "    " << (hatStates[i] ? "on" : "off") << std::endl;
                }
                std::cout << "  ]" << std::endl;
            }
        }
    }
} // namespace

struct WindowData {
    std::shared_ptr<SceneView> sceneView;
    GLFWwindow* window;
    ProgramResource drawProgram;
    glm::vec2 aspect;
    Uniform<glm::vec2> aspectUniform;
    VertexArray vao;
    Buffer vertexData;
    std::vector<std::shared_ptr<IMouseListener>> mouseListeners;
    std::vector<std::shared_ptr<IKeyboardListener>> keyboardListeners;
};

Window::Window(const std::string& title, const RectSize<int> size)
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
    glfwWindowHint(GLFW_DECORATED, false);
    
    GLFWwindow* window = glfwCreateWindow(size.width, size.height, title.c_str(), NULL, NULL);
    if (window == nullptr) {
        throw std::runtime_error("Could not create GLFW window.");
    }

    glfwSetWindowUserPointer(window, this);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    _data = std::make_unique<WindowData>();
    _data->window = window;

    _data->drawProgram.buildWithSource(kVertexShader, kFragmentShader);
    ProgramContext progContext(_data->drawProgram);
    {
        _data->aspectUniform = _data->drawProgram.getUniform<glm::vec2>("aspect");
        _data->drawProgram.getUniform<GLint>("inputTexture").set(0);
    }
    glfwSetInputMode(_data->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(_data->window, [](GLFWwindow* window, int, int) {
        Window* myThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myThis->updateAspect();
    });
    updateAspect();

    glfwSetCursorPosCallback(_data->window, [](GLFWwindow* window, double xPos, double yPos) {
        Window* myThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        Point<double> position{ xPos, yPos };
        for (const auto& listener : myThis->_data->mouseListeners) {
            listener->moved(position);
        }
    });

    glfwSetMouseButtonCallback(
        _data->window, [](GLFWwindow* window, int glfwButton, int action, int) {
            Window* myThis = static_cast<Window*>(glfwGetWindowUserPointer(window));

            MouseButton button;
            switch (glfwButton) {
            case GLFW_MOUSE_BUTTON_LEFT:
                button = MouseButton::Left;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                button = MouseButton::Right;
                break;
            default:
                return;
            }

            switch (action) {
            case GLFW_PRESS:
                for (const auto& listener : myThis->_data->mouseListeners) {
                    listener->buttonPressed(button);
                }
                break;
            case GLFW_RELEASE:
                for (const auto& listener : myThis->_data->mouseListeners) {
                    listener->buttonReleased(button);
                }
            default:
                return;
            }
        });

    glfwSetScrollCallback(_data->window, [](GLFWwindow* window, double xOffset, double yOffset) {
        Window* myThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        for (const auto& listener : myThis->_data->mouseListeners) {
            listener->scrolled(xOffset, yOffset);
        }
    });

    glfwSetKeyCallback(_data->window, [](GLFWwindow* window, int glfwKey, int, int action, int) {
        Window* myThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
        KeyboardKey key;
        switch (glfwKey) {
        case GLFW_KEY_W:
            key = KeyboardKey::W;
            break;
        case GLFW_KEY_S:
            key = KeyboardKey::S;
            break;
        case GLFW_KEY_A:
            key = KeyboardKey::A;
            break;
        case GLFW_KEY_D:
            key = KeyboardKey::D;
            break;
        case GLFW_KEY_SPACE:
            key = KeyboardKey::Space;
            break;
        default:
            return;
        }

        switch (action) {
        case GLFW_PRESS:
            for (const auto& listener : myThis->_data->keyboardListeners) {
                listener->keyPressed(key);
            }
            break;
        case GLFW_RELEASE:
            for (const auto& listener : myThis->_data->keyboardListeners) {
                listener->keyReleased(key);
            }
        }
        for (const auto& listener : myThis->_data->keyboardListeners) {
        }
    });

    dumpJoysticks();

    VertexArrayContext context(_data->vao);
    context.setBuffer<GL_ARRAY_BUFFER>(_data->vertexData);

    context.bindBufferData<GL_ARRAY_BUFFER>(gsl::span(kFullScreenQuadVertices), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
}

Window::~Window() {}

void Window::setSceneView(std::shared_ptr<SceneView> sceneView)
{
    _data->sceneView = std::move(sceneView);
    updateAspect();
}

void Window::makeCurrent() { glfwMakeContextCurrent(_data->window); }

void Window::draw()
{
    if (_data->sceneView == nullptr) {
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
    if (_data->sceneView == nullptr) {
        return;
    }

    RectSize<GLsizei> contentSize = _data->sceneView->getOutputSize();
    if (contentSize.height == 0 || contentSize.width == 0) {
        return;
    }

    RectSize<GLsizei> frameBufferSize;
    glfwGetFramebufferSize(_data->window, &frameBufferSize.width, &frameBufferSize.height);

    float contentAspect
        = static_cast<float>(contentSize.width) / static_cast<float>(contentSize.height);
    float frameBufferAspect
        = static_cast<float>(frameBufferSize.width) / static_cast<float>(frameBufferSize.height);

    if (frameBufferAspect > contentAspect) {
        _data->aspect = { (contentAspect / frameBufferAspect), 1.0f };
    } else {
        _data->aspect = { 1.0f, (frameBufferAspect / contentAspect) };
    }
}

bool Window::wantsClose() { return glfwWindowShouldClose(_data->window); }

Point<double> Window::getMousePosition()
{
    Point<double> position;
    glfwGetCursorPos(_data->window, &position.x, &position.y);
    return position;
}

void Window::addMouseListener(std::shared_ptr<IMouseListener> listener)
{
    _data->mouseListeners.push_back(std::move(listener));
}

void Window::addKeyboardListener(std::shared_ptr<IKeyboardListener> listener)
{
    _data->keyboardListeners.push_back(std::move(listener));
}

} // namespace rev