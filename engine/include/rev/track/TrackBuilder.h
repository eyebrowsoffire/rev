#pragma once

#include "rev/NurbsCurve.h"
#include "rev/track/ITrackElement.h"

namespace rev {

struct TrackConfiguration {
    NurbsCurve<glm::vec3> curve;
    float width;
    size_t segments;
};

void buildTrack(const TrackConfiguration& config, ITrackElement& element);

}