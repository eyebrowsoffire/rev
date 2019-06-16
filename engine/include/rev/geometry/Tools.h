#pragma once

#include <glm/glm.hpp>
#include <gsl/gsl>

namespace rev {

struct Segment {
    glm::vec3 start;
    glm::vec3 end;
};

template <typename VertexRange, typename SegmentVisitor>
void iteratePolygonVertices(const VertexRange& vertices, SegmentVisitor&& visitor)
{
    Expects(vertices.size() >= 3);
    auto begin = std::begin(vertices);
    auto end = std::end(vertices);

    auto segmentStart = begin;
    auto segmentEnd = begin + 1;

    while (segmentStart != end) {
        if (segmentEnd == end) {
            segmentEnd = begin;
        }

        visitor(Segment{ *segmentStart, *segmentEnd });
        segmentStart++;
        segmentEnd++;
    }
}

class NullPolygonBuilder {
public:
    void addVertex(const glm::vec3& vertex) {}
};

template <typename IteratorType>
class OutputIteratorPolygonBuilder {
public:
    OutputIteratorPolygonBuilder(IteratorType iterator)
        : _iter(iterator)
    {
    }

    void addVertex(const glm::vec3& vertex)
    {
        *_iter = vertex;
        _iter++;
    }

    IteratorType getIterator() const { return _iter; }

private:
    IteratorType _iter;
};

struct AxisAlignedPlane {
    glm::vec3 intersect(const Segment& segment) const
    {
        glm::vec3 segmentDelta = segment.end - segment.start;
        float boundaryDelta = boundary - segment.start[dimensionIndex];
        float lengthRatio = boundaryDelta / segmentDelta[dimensionIndex];

        glm::vec3 intersectPoint = segment.start;
        intersectPoint += lengthRatio * segmentDelta;

        // Avoid floating point error along the boundary dimension.
        intersectPoint[dimensionIndex] = boundary;
        return intersectPoint;
    }

    template <typename InputVertexRange, typename LeftBuilder, typename RightBuilder>
    void splitConvexPolygon(const InputVertexRange& inputVertices, LeftBuilder&& leftBuilder,
        RightBuilder&& rightBuilder) const
    {
        iteratePolygonVertices(
            inputVertices, [this, &leftBuilder, &rightBuilder](const Segment& segment) {
                float startPosition = segment.start[dimensionIndex];
                float endPosition = segment.end[dimensionIndex];

                bool emitIntersection = false;
                if (startPosition < boundary) {
                    leftBuilder.addVertex(segment.start);
                    if (endPosition > boundary) {
                        emitIntersection = true;
                    }
                } else if (startPosition > boundary) {
                    rightBuilder.addVertex(segment.start);
                    if (endPosition < boundary) {
                        emitIntersection = true;
                    }
                } else {
                    // Right on the boundary.
                    leftBuilder.addVertex(segment.start);
                    rightBuilder.addVertex(segment.start);
                }

                if (emitIntersection) {
                    auto intersection = intersect(segment);
                    leftBuilder.addVertex(intersection);
                    rightBuilder.addVertex(intersection);
                }
            });
    }

    uint8_t dimensionIndex;
    float boundary;
};

struct AxisAlignedBoundingBox {
    void expandToVertex(const glm::vec3& vertex)
    {
        for (int k = 0; k < 3; k++) {
            if (vertex[k] < minimum[k]) {
                minimum[k] = vertex[k];
            }
            if (vertex[k] > maximum[k]) {
                maximum[k] = vertex[k];
            }
        }
    }

    void expandToBox(const AxisAlignedBoundingBox& other)
    {
        for (int k = 0; k < 3; k++) {
            if (other.minimum[k] < minimum[k]) {
                minimum[k] = other.minimum[k];
            }
            if (other.maximum[k] > maximum[k]) {
                maximum[k] = other.maximum[k];
            }
        }
    }

    float getSurfaceArea() const
    {
        glm::vec3 diagonal = maximum - minimum;
        float surfaceArea = 0.0f;
        surfaceArea += diagonal.x * diagonal.y;
        surfaceArea += diagonal.y * diagonal.z;
        surfaceArea += diagonal.z * diagonal.z;

        return 2.0f * surfaceArea;
    }

    float getVolume() const {
        glm::vec3 diagonal = maximum - minimum;
        return diagonal.x * diagonal.y * diagonal.z;
    }

    std::array<AxisAlignedBoundingBox, 2> split(const AxisAlignedPlane& plane) const
    {
        Expects(!(plane.boundary < minimum[plane.dimensionIndex]));
        Expects(!(plane.boundary > maximum[plane.dimensionIndex]));

        std::array<AxisAlignedBoundingBox, 2> splitBoxes{ *this, *this };
        splitBoxes[0].maximum[plane.dimensionIndex] = plane.boundary;
        splitBoxes[1].minimum[plane.dimensionIndex] = plane.boundary;

        return splitBoxes;
    }

    glm::vec3 minimum{ std::numeric_limits<float>::infinity() };
    glm::vec3 maximum{ -std::numeric_limits<float>::infinity() };
};

template <typename VertexRange>
AxisAlignedBoundingBox smallestBoxContainingVertices(const VertexRange& range)
{
    AxisAlignedBoundingBox box;
    for (const auto& vertex : range) {
        box.expandToVertex(vertex);
    }
    return box;
}

}