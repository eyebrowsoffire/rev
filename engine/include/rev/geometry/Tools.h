#pragma once

#include <glm/glm.hpp>
#include <gsl/gsl>
#include <optional>

namespace rev {

struct Segment {
    glm::vec3 start;
    glm::vec3 end;
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Sphere {
    glm::vec3 center;
    float radius;
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
    struct Hit {
        glm::vec3 intersectionPoint;
        float t;
    };

    Hit intersect(const Ray& ray) const
    {
        float boundaryDelta = boundary - ray.origin[dimensionIndex];
        float t = boundaryDelta / ray.direction[dimensionIndex];

        glm::vec3 intersectPoint = ray.origin + (t * ray.direction);

        // Avoid floating point error along the boundary dimension.
        intersectPoint[dimensionIndex] = boundary;
        return { intersectPoint, t };
    }

    glm::vec3 intersect(const Segment& segment) const
    {
        glm::vec3 direction = segment.end - segment.start;
        return intersect(Ray{ segment.start, direction }).intersectionPoint;
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

    float getVolume() const
    {
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

    struct Hit {
        glm::vec3 intersectionPoint;
        float t;
        uint8_t planeDimension;
    };

    bool containsPoint(const glm::vec3& point) const
    {
        for (int k = 0; k < 3; k++) {
            if (point[k] < minimum[k]) {
                return false;
            }
            if (point[k] > maximum[k]) {
                return false;
            }
        }

        return true;
    }

    bool intersectsSphere(const Sphere& sphere) const
    {
        float r2 = sphere.radius * sphere.radius;
        for (size_t k = 0; k < 3; k++) {
            if (sphere.center[k] < minimum[k]) {
                float diff = minimum[k] - sphere.center[k];
                r2 -= diff * diff;
            } else if (sphere.center[k] > maximum[k]) {
                float diff = sphere.center[k] - maximum[k];
                r2 -= diff * diff;
            }
        }
        return r2 > 0.0f;
    }

    std::optional<Hit> castExternalRay(const Ray& ray) const
    {
        for (uint8_t k = 0; k < 3; k++) {
            float d = ray.direction[k];
            if (d < 0.0f) {
                float boundary = maximum[k];
                if (ray.origin[k] > boundary) {
                    auto intersection = AxisAlignedPlane{ k, boundary }.intersect(ray);
                    if (containsPoint(intersection.intersectionPoint)) {
                        return Hit{ intersection.intersectionPoint, intersection.t, k };
                    }
                }
            } else if (d > 0.0f) {
                float boundary = minimum[k];
                if (ray.origin[k] < boundary) {
                    auto intersection = AxisAlignedPlane{ k, boundary }.intersect(ray);
                    if (containsPoint(intersection.intersectionPoint)) {
                        return Hit{ intersection.intersectionPoint, intersection.t, k };
                    }
                }
            }
        }
        return std::nullopt;
    }

    Hit castInternalRay(const Ray& ray) const
    {
        for (uint8_t k = 0; k < 3; k++) {
            float d = ray.direction[k];

            float boundary;
            if (d < 0.0f) {
                boundary = minimum[k];
            } else if (d > 0.0f) {
                boundary = maximum[k];
            } else {
                continue;
            }

            auto intersection = AxisAlignedPlane{ k, boundary }.intersect(ray);
            if (containsPoint(intersection.intersectionPoint)) {
                return Hit{ intersection.intersectionPoint, intersection.t, k };
            }
        }
        throw std::runtime_error("Internal ray intersection not found.");
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