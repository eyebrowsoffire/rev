#pragma once

#include "rev/Camera.h"
#include "rev/DrawMaterialsProgram.h"
#include "rev/Mesh.h"

#include <vector>

namespace rev {

class ProgramFactory;

struct TrackObject {
};

class TrackModel {
public:
    using SceneObjectType = TrackObject;

    TrackModel(ProgramFactory& factory, Mesh trackMesh);

    void render(Camera& camera, gsl::span<std::shared_ptr<TrackObject>> objects);
private:
    std::shared_ptr<DrawMaterialsProgram> _program;
    Mesh _trackMesh;
};
}; // namespace rev