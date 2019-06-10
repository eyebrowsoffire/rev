#pragma once

#include "rev/Utilities.h"
#include <array>
#include <glm/glm.hpp>
#include <gsl/gsl_assert>
#include <limits>
#include <set>
#include <variant>
#include <vector>

namespace rev {

struct AxisAlignedPlane {
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

    std::pair<AxisAlignedBoundingBox, AxisAlignedBoundingBox> split(
        const AxisAlignedPlane& plane) const
    {
        Expects(!(plane.boundary < minimum[plane.dimensionIndex]));
        Expects(!(plane.boundary > maximum[plane.dimensionIndex]));

        std::pair<AxisAlignedBoundingBox, AxisAlignedBoundingBox> splitBoxes{ *this, *this };
        splitBoxes.first.maximum[plane.dimensionIndex] = plane.boundary;
        splitBoxes.second.minimum[plane.dimensionIndex] = plane.boundary;

        return splitBoxes;
    }

    glm::vec3 minimum{ std::numeric_limits<float>::infinity() };
    glm::vec3 maximum{ -std::numeric_limits<float>::infinity() };
};

template <typename SegmentVisitor>
void iteratePolygonVertices(gsl::span<glm::vec3> vertices, SegmentVisitor&& visitor)
{
    Expects(vertices.size() >= 3);
    size_t vertexCount = vertices.size();
    size_t i = 0;
    size_t j = 1;
    while (i < vertexCount) {
        if (j == vertexCount) {
            j = 0;
        }
        visitor(vertices[i], vertices[j]);
        i++;
        j++;
    }
}

glm::vec3 intersect(const glm::vec3& first, const glm::vec3& second, const AxisAlignedPlane& plane)
{
    
    for (size_t k; k < 3; k++) {
        if (k == plane.dimensionIndex) {
        }
    }
}

template <typename SurfaceData>
struct Triangle {
    std::array<glm::vec3, 3> vertices;
    SurfaceData data;

    AxisAlignedBoundingBox getBoundingBox() const
    {
        AxisAlignedBoundingBox box;
        for (const auto& vertex : vertices) {
            box.expandToVertex(vertex);
        }
        return box;
    }

    AxisAlignedBoundingBox getClippedBoundingBox(const AxisAlignedBoundingBox& clipBox) const
    {
        std::vector<glm::vec3> oldVertices = vertices;
        std::vector<glm::vec3> newVertices;
        iteratePolygonVertices(
            oldVertices, [&newVertices](const glm::vec3& first, const glm::vec3& second) {
                bool firstOutside = (first.x < clipbox.minimum.x);
                bool secondOutside = (second.x < clipbox.minimum.x);
                if (!firstOutside) {
                    newVertices.push_back(first);
                }

                if (firstOutside != secondOutside) {
                }
                if (first < clipBox.minimum.x) {
                    if (!(second < clipBox.minimum.x)) {
                    }
                } else {
                    newVertices.push_back(first);
                }
            });
    }
};

template <typename SurfaceData>
struct LeafNode {
    std::unordered_set<Triangle<SurfaceData>*> triangles;
};

template <typename SurfaceData, size_t axisIndex>
struct BranchNode;

template <typename SurfaceData>
using MapNode = std::variant<LeafNode<SurfaceData>, BranchNode<SurfaceData, 0>,
    BranchNode<SurfaceData, 1>, BranchNode<SurfaceData, 2>>;

template <typename SurfaceData, size_t axisIndex>
struct BranchNode {
    AxisAlignedPlane split;

    std::unique_ptr<MapNode<SurfaceData>> left;
    std::unique_ptr<MapNode<SurfaceData>> right;
};

template <typename SurfaceData>
class SurfaceMap {
public:
    SurfaceMap(std::vector<std::unique_ptr<Triangle<SurfaceData>>> triangles,
        std::unique_ptr<MapNode<SurfaceData>> root, const AxisAlignedBoundingBox boundingBox)
        : _triangles(std::move(triangles))
        , _root(std::move(root))
        , _boundingBox(boundingBox)
    {
    }

private:
    std::vector<std::unique_ptr<Triangle<SurfaceData>>> _triangles;
    std::unique_ptr<MapNode<SurfaceData>> _root;
    AxisAlignedBoundingBox _boundingBox;
};

template <typename SurfaceData>
class SurfaceMapBuilder {
public:
    void addTriangle(std::array<glm::vec3, 3> vertices, SurfaceData data)
    {
        auto triangle = std::make_unique<Triangle>({ vertices, data });
        auto boundingBox = triangle->getBoundingBox();
        _boundingBox.expandToBox(boundingBox);

        std::set<Event> events;
        for (int k = 0; k < 3; k++) {
            float minimum = boundingBox.minimum[k];
            float maximum = boundingBox.maximum[k];
            if (minimum < maximum) {
                events.emplace({ triangle.get(),
                    AxisAlignedPlane{ static_cast<uint8_t>(k), maximum }, Event::Type::Ending });
                events.emplace({ triangle.get(),
                    AxisAlignedPlane{ static_cast<uint8_t>(k), minimum }, Event::Type::Starting });
            } else {
                events.emplace({ triangle.get(),
                    AxisAlignedPlane{ static_cast<uint8_t>(k), minimum }, Event::Type::Planar });
            }
        }
        _events.merge(std::move(events));
        _triangles.push_back(std::move(triangle));
    }

    SurfaceMap<SurfaceData> build()
    {
        Expects(!_triangles.empty());
        Expects(!_events.empty());

        std::unordered_set<Triangle<SurfaceData>*> triangleSet;
        for (const auto& triangle : _triangles) {
            triangleSet.push_back(triangle.get());
        }
        auto rootNode = createNode(_boundingBox, std::move(_events), triangleSet);
        return SurfaceMap<SurfaceData>{ std::move(_triangles), std::move(rootNode), _boundingBox };
    }

private:
    static constexpr float kTraversalCost = 15.0f;
    static constexpr float kIntersectionCost = 20.0f;
    static constexpr float kEmptySplitDiscount = 0.8f;

    enum class Side {
        Left,
        Right,
    };

    struct Event {
        enum class Type {
            Ending = 0,
            Planar = 1,
            Starting = 2,
        };

        bool operator<(const SurfaceMapBuilder& other) const
        {
            auto tuplify = [](const Event& event) {
                return std::tie(event.separationPlane.boundary, event.separationPlane.dimension,
                    event.type, event.triangle);
            };
            return tuplify(*this) < tuplify(other);
        }

        Triangle<SurfaceData>* triangle;
        AxisAlignedPlane separationPlane;
        Type type;
    };

    std::unique_ptr<MapNode<SurfaceData>> createNode(const AxisAlignedBoundingBox& box,
        std::set<Event> events, std::unordered_set<Triangle<SurfaceData>*> triangles)
    {
        auto [plane, side, cost] = findBestSplit(box, events, triangles.size());
        float terminateCost = static_cast<float>(triangleCount) * kIntersectionCost;
        if (terminateCost > cost) {
            std::unordered_set<Triangle<SurfaceData>*> leftTriangles;
            std::unordered_set<Triangle<SurfaceData>*> rightTriangles;

            for (const auto& event : events) {
                const auto& eventPlane = event.separationPlane;
                if (eventPlane.dimensionIndex != plane.dimensionIndex) {
                    continue;
                }

                if (eventPlane.boundary < plane.boundary) {
                    if (event.type != Event::Type::Starting) {
                        triangles.remove(event.triangle);
                        leftTriangles.insert(event.triangle);
                    }
                } else if (eventPlane.boundary > plane.boundary) {
                    if (event.type != Event::Type::Ending) {
                        triangles.remove(event.triangle);
                        rightTriangles.insert(event.triangle);
                    }
                } else {
                    // On the boundary
                    triangles.remove(event.triangle);
                    switch (event.type) {
                    case Event::Type::Starting:
                        rightTriangles.insert(event.triangle);
                        break;
                    case Event::Type::Ending:
                        leftTriangles.insert(event.triangle);
                        break;
                    case Event::Type::Planar:
                        if (side == Side::Left) {
                            leftTriangles.insert(event.triangle);
                        } else {
                            rightTriangles.insert(event.triangle);
                        }
                        break;
                    }
                }
            }

            std::set<Event> leftEvents;
            std::set<Event> rightEvents;
            for (const auto& event : events) {
                if (leftTriangles.contains(event.triangle)) {
                    leftEvents.push(event);
                } else if (rightTriangles.contains(event.triangle)) {
                    rightEvents.push(event);
                }
            }
            events.clear();

            // Overlapping triangles remain
            for (const auto& triangle : triangles) {
                leftTriangles.push(triangle);
                rightTriangles.push(triangle);
            }
            triangles.clear();
        } else {
            return std::make_unique<MapNode<SurfaceData>>(LeafNode{ std::move(triangles) });
        }
    }

    std::tuple<AxisAlignedPlane, Side, float> findBestSplit(
        const AxisAlignedBoundingBox& box, const std::set<Event>& events, size_t triangleCount)
    {
        float bestCost = std::numeric_limits<float>::infinity();
        const AxisAlignedPlane* bestPlane;
        Side bestSide;

        glm::uvec3 leftTriangleCounts{ 0 };
        glm::uvec3 rightTriangleCounts{ static_cast<unsigned int>(triangleCount) };
        auto iter = events.begin();
        auto eventsEnd = events.end();
        while (iter != eventsEnd) {
            const AxisAlignedPlane& candidatePlane = iter->separationPlane;
            uint8_t dimension = candidatePlane.dimensionIndex;
            size_t endingTriangleCount = 0;
            size_t planarTriangleCount = 0;
            size_t startingTriangleCount = 0;
            auto validate = [&eventsEnd, &candidatePlane, dimension](const auto& iter) {
                return (iter != eventsEnd)
                    && !(iter->separationPlane.candidatePlane.boundary > candidatePlane.boundary)
                    && (iter->separationPlane.candidatePlane.dimensionIndex == dimension);
            };
            while (validate(iter) && (iter->type == Event::Type::Ending)) {
                endingTriangleCount++;
                iter++;
            }
            while (validate(iter) && (iter->type == Event::Type::Planar)) {
                planarTriangleCount++;
                iter++;
            }
            while (validate(iter) && (iter->type == Event::Type::Starting)) {
                startingTriangleCount++;
                iter++;
            }
            rightTriangleCounts[dimension] -= planarTriangleCount;
            rightTriangleCounts[dimension] -= endingTriangleCount;
            auto [side, cost] = surfaceAreaCostHeuristic(box, candidatePlane,
                leftTriangleCounts[dimension], planarTriangleCount, rightTriangleCounts[dimension]);
            if (cost < bestCost) {
                bestCost = cost;
                bestPlane = &candidatePlane;
                bestSide = side;
            }
            leftTriangleCounts[dimension] += planarTriangleCount;
            leftTriangleCounts[dimension] += startingTriangleCount;
        }
        return { *bestPlane, bestSide, bestCost };
    }

    std::pair<Side, float> surfaceAreaCostHeuristic(const AxisAlignedBoundingBox& box,
        const AxisAlignedPlane& splitPlane, size_t leftTriangleCount, size_t planarTriangleCount,
        size_t rightTriangleCount)
    {
        auto splitBoxes = box.split(splitPlane);
        float boxSurfaceArea = box.getSurfaceArea();

        float leftArea = splitBoxes.first.getSurfaceArea() / boxSurfaceArea;
        float rightArea = splitBoxes.second.getSurfaceArea() / boxSurfaceArea;

        auto cost = [leftArea, rightArea](size_t leftTriangleCount, size_t rightTriangleCount) {
            float cost = static_cast<float>(leftTriangleCount) * leftArea
                + static_cast<float>(rightTriangleCount) * rightArea;
            if (!leftTriangleCount || !rightTriangleCount) {
                // We slightly prefer splits that remove empty space.
                cost *= kEmptySplitDiscount;
            }
            return cost;
        };

        float leftCost = cost(leftTriangleCount + planarTriangleCount, rightTriangleCount);
        float rightCost = cost(leftTriangleCount, planarTriangleCount + rightTriangleCount);
        if (leftCost < rightCost) {
            return { Side::Left, (kIntersectionCost * leftCost) + kTraversalCost };
        } else {
            return { Side::Right, (kIntersectionCost * rightCost) + kTraversalCost };
        }
    }

    AxisAlignedBoundingBox _boundingBox;
    std::vector<std::unique_ptr<Triangle<SurfaceData>>> _triangles;
    std::set<Event> _events;
};
}