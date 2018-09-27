#include "rev/SceneView.h"

#include "rev/Camera.h"
#include "rev/Scene.h"

#include <OpenGL/gl3.h>

namespace rev
{

namespace {
    constexpr const char* kVertexShader = R"vertexShader(
        #version 330 core

        layout(location = 0) in vec3 vPosition;
        layout(location = 1) in vec3 vNormal;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec3 normal;

        void main()
        {
            gl_Position = projection * view * model * vec4(vPosition, 1.0);
            normal = vNormal;
        }
    )vertexShader";

    constexpr const char* kFragmentShader = R"fragmentShader(
        #version 330 core

        in vec3 normal;

        out vec4 fragColor;
        void main() 
        {
            fragColor = vec4(abs(normal), 1.0);
        }
    )fragmentShader";
}

SceneView::SceneView()
: _camera(std::make_shared<Camera>())
{
    VertexShader vShader;
    vShader.setSource(kVertexShader);
    vShader.compile();
    if(!vShader.getCompileStatus())
    {
        throw vShader.getCompileLog();
    }

    FragmentShader fShader;
    fShader.setSource(kFragmentShader);
    fShader.compile();
    if(!fShader.getCompileStatus())
    {
        throw fShader.getCompileLog();
    }

    _program.attachShader(vShader);
    _program.attachShader(fShader);
    _program.link();
    if(!_program.getLinkStatus())
    {
        throw _program.getLinkLog();
    }

    _model = _program.getUniform<glm::mat4>("model");
    _view = _program.getUniform<glm::mat4>("view");
    _projection = _program.getUniform<glm::mat4>("projection");
}

void SceneView::setScene(std::shared_ptr<Scene> scene)
{
    _scene = std::move(scene);
}

const std::shared_ptr<Camera>& SceneView::getCamera() const
{
    return _camera;
}

void SceneView::render()
{
    if(!_scene)
    {
        return;
    }

    ProgramContext programContext(_program);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    _view.set(_camera->getViewMatrix());
    _projection.set(_camera->getProjectionMatrix());

    _scene->renderAllObjects(_model);
}

} // namespace rev