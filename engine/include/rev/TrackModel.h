#pragma once

#include "rev/Camera.h"
#include "rev/DrawMaterialsProgram.h"
#include "rev/Mesh.h"
#include "rev/NurbsCurve.h"
#include "rev/geometry/KDTree.h"
#include "rev/gl/Buffer.h"

#include <vector>

namespace rev {

class ProgramFactory;

struct TrackObject {
};

class TrackBuilder;

class TrackModel {
public:
    using SceneObjectType = TrackObject;

    TrackModel(
        ProgramFactory& factory, const NurbsCurve<glm::vec3>& curve, float width, size_t segments);

    void render(Camera& camera, gsl::span<std::shared_ptr<TrackObject>> objects);

    struct SurfaceData {
    };

    const KDTree<SurfaceData>& getSurfaceMap() const { return _surfaceMap; }

private:
    friend class TrackBuilder;

    TrackModel(ProgramFactory& factory, TrackBuilder&& model);

    std::shared_ptr<DrawMaterialsProgram> _program;
    Mesh _trackMesh;
    KDTree<SurfaceData> _surfaceMap;
};
}; // namespace rev