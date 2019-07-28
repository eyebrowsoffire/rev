#pragma once

#include <glm/glm.hpp>

namespace rev {

class ITrackElement {
public:
    virtual ~ITrackElement() = default;

    // This is called over and over by the track builder as the track is generated.
    // The orientation matrix shows the position and orientation of the next piece of track.
    virtual void stamp(const glm::mat4 &orientation) = 0;

    // Called when the track builder is at the end of the track.
    virtual void finish() = 0;
};

}
