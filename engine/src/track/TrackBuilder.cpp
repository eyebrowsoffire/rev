#include "rev/track/TrackBuilder.h"

#include <gsl/gsl_assert>

namespace rev {

void buildTrack(const TrackConfiguration& config, ITrackElement& element)
{
    Expects(config.segments > 0);

    float start = config.curve.getStart();
    float end = config.curve.getEnd();
    float curveLength = end - start;
    float segmentLength = curveLength / static_cast<float>(config.segments);

    std::vector<glm::vec3> guidePoints;
    for (float current = start; current < end; current += segmentLength) {
        guidePoints.push_back(config.curve[current]);
    }

    auto firstGuidePoint = guidePoints.begin();
    auto secondGuidePoint = firstGuidePoint + 1;
    auto guidePointsEnd = guidePoints.end();
    glm::vec3 previousLeftPosition;
    glm::vec3 previousRightPosition;
    size_t previousLeftIndex;
    size_t previousRightIndex;

    while (secondGuidePoint != guidePointsEnd) {
        glm::vec3 forward = glm::normalize(*firstGuidePoint - *secondGuidePoint);
        glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 up = glm::cross(forward, right);

        glm::vec3 midPoint = (*firstGuidePoint + *secondGuidePoint) / 2.0f;

        glm::mat4 orientation{
            { right * config.width, 0.0f },
            { up, 0.0f },
            { forward, 0.0f },
            { midPoint, 1.0f },
        };

        element.stamp(orientation);

        firstGuidePoint = secondGuidePoint;
        secondGuidePoint++;
    }
    element.finish();
}

}