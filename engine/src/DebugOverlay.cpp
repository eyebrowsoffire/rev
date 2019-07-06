#include "rev/DebugOverlay.h"

#include "rev/ProgramFactory.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"

namespace rev {
class DebugOverlayProgram {
public:
    DebugOverlayProgram(ProgramResource resource)
        : _resource(std::move(resource))
    {
        view = _resource.getUniform<glm::mat4>("view");
        projection = _resource.getUniform<glm::mat4>("projection");
    }

    Uniform<glm::mat4> view;
    Uniform<glm::mat4> projection;

    ProgramContext prepareContext() { return ProgramContext(_resource); }

    struct Source {
        static std::string_view getVertexSource()
        {
            return R"vertexShader(
#version 330 core

layout(location = 0) in vec3 vPosition;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldSpacePosition = vec4(vPosition, 1.0f);
    gl_Position = projection * view * worldSpacePosition;
}
)vertexShader";
        }

        static std::string_view getFragmentSource()
        {
            return R"fragmentShader(
#version 330 core

out vec4 fragColor;

void main() 
{
    fragColor = vec4(1.0, 0.0, 0.0, 0.1);
}
)fragmentShader";
        }
    };

private:
    ProgramResource _resource;
};

DebugOverlay::DebugOverlay()
{
    VertexArrayContext context(_vao);
    context.setBuffer<GL_ARRAY_BUFFER>(_buffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void DebugOverlay::setTriangle(gsl::span<const glm::vec3, 3> vertices)
{
    VertexArrayContext context{ _vao };
    context.bindBufferData<GL_ARRAY_BUFFER>(vertices, GL_DYNAMIC_DRAW);

    _shouldRender = true;
}

void DebugOverlay::clearTriangle() { _shouldRender = false; }

void DebugOverlay::render()
{
    if (!_shouldRender) {
        return;
    }
    VertexArrayContext context(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

DebugOverlayModel::DebugOverlayModel(ProgramFactory& factory)
    : _program(factory.getProgram<DebugOverlayProgram>())
{
}

void DebugOverlayModel::render(Camera& camera, gsl::span<std::shared_ptr<DebugOverlay>> objects)
{
    auto programContext = _program->prepareContext();
    _program->view.set(camera.getViewMatrix());
    _program->projection.set(camera.getProjectionMatrix());
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    for (const auto& object : objects) {
        object->render();
    }
}

}