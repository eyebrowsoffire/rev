#include "rev/track/ExtrusionTrackElement.h"

namespace rev {

ExtrusionTrackElement::ExtrusionTrackElement(DieTemplate dieTemplate)
    : _dieTemplate(std::move(dieTemplate))
{
    Expects(_dieTemplate.vertices.size());
    Expects(_dieTemplate.edges.size());
}

void ExtrusionTrackElement::stamp(const glm::mat4& orientation)
{
    std::vector<size_t> stampIndices;
    for (const auto& vertex : _dieTemplate.vertices) {
        glm::vec3 position = (orientation * glm::vec4(vertex.position, 1.0f));
        glm::vec3 normal = (orientation * glm::vec4(vertex.normal, 0.0f));
        size_t index = _meshBuilder.pushVertex(position, normal);
        stampIndices.push_back(index);
    }

    if (_previousStampIndices.size()) {
        Expects(stampIndices.size() == _previousStampIndices.size());
        for (const auto& edge : _dieTemplate.edges) {
            size_t right = stampIndices[edge[0]];
            size_t left = stampIndices[edge[1]];
            size_t prevRight = _previousStampIndices[edge[0]];
            size_t prevLeft = _previousStampIndices[edge[1]];

            _meshBuilder.emitTriangle(right, left, prevRight);
            _meshBuilder.emitTriangle(left, prevLeft, prevRight);
        }
    }

    _previousStampIndices = std::move(stampIndices);
}

void ExtrusionTrackElement::finish() {}

Mesh ExtrusionTrackElement::buildMesh() {
    return _meshBuilder.createMesh();
}
}