#pragma once

#include "rev/Mesh.h"
#include "rev/track/ITrackElement.h"
#include "rev/track/TrackVertexData.h"

#include <vector>

namespace rev {

// Represents the cross-section (die cut) to extrude through the track curve.
struct DieTemplate {
    // All the vertices in this cross-section
    std::vector<TrackVertexData> vertices;

    // Edges of the actual die, represented by pairs of indexes into the vertices vector.
    std::vector<std::array<size_t, 2>> edges;
};

class ExtrusionTrackElement : public ITrackElement {
public:
    ExtrusionTrackElement(DieTemplate dieTemplate);
    void stamp(const glm::mat4& orientation) override;
    void finish() override;

    MeshBuilder<TrackVertexData>& getMeshBuilder();

private:
    DieTemplate _dieTemplate;
    MeshBuilder<TrackVertexData> _meshBuilder;
    std::vector<size_t> _previousStampIndices;
};

}