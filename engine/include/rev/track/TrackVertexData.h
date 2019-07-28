#pragma once

#include "rev/gl/VertexArray.h"

#include <glm/glm.hpp>

namespace rev {
struct TrackVertexData {
    TrackVertexData(const glm::vec3& positionArg, const glm::vec3& normalArg)
        : position(positionArg)
        , normal(normalArg)
    {
    }

    glm::vec3 position;
    glm::vec3 normal;

    static void setupAttributes(VertexArrayContext& context)
    {
        context.setupVertexAttribute<decltype(position)>(
            0, offsetof(TrackVertexData, position), sizeof(TrackVertexData));
        context.setupVertexAttribute<decltype(normal)>(
            1, offsetof(TrackVertexData, normal), sizeof(TrackVertexData));
    }
};

}