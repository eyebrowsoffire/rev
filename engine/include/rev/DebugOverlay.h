#pragma once

#include "rev/Camera.h"
#include "rev/ProgramFactory.h"
#include "rev/gl/Buffer.h"
#include "rev/gl/VertexArray.h"

namespace rev {
class DebugOverlay {
public:
    DebugOverlay();
    
    void setTriangle(const gsl::span<glm::vec3, 3> vertices);
    void clearTriangle();

    void render();

private:
    VertexArray _vao;
    Buffer _buffer;
    bool _shouldRender = false;
};

class DebugOverlayProgram;

class DebugOverlayModel {
public:
    using SceneObjectType = DebugOverlay;

    DebugOverlayModel(ProgramFactory& factory);
    void render(Camera& camera, gsl::span<std::shared_ptr<DebugOverlay>> objects);

private:
    std::shared_ptr<DebugOverlayProgram> _program;
};
}
