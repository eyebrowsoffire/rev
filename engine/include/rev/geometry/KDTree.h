#pragma once

#include "rev/Utilities.h"
#include "rev/geometry/Tools.h"
#include <array>
#include <glm/glm.hpp>
#include <gsl/gsl_assert>
#include <iostream>
#include <limits>
#include <set>
#include <unordered_set>
#include <variant>
#include <vector>

namespace rev {

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

    std::array<AxisAlignedBoundingBox, 2> clipAndSplitBoundingBoxes(
        const AxisAlignedBoundingBox& clipBox, const AxisAlignedPlane& splitPlane)
    {
        // Alternate between these two buffers for input and output
        std::array<glm::vec3, 9> buf1{};
        std::array<glm::vec3, 9> buf2{};

        // Fill the input buffer
        for (int i = 0; i < 3; i++) {
            buf1[i] = vertices[i];
        }

        // Clip the triangle by all sides of our bounding box
        auto inputIter = buf1.begin();
        auto inputEnd = inputIter + 3;
        auto outputIter = buf2.begin();

        // First the min planes in each dimension
        for (uint8_t dim = 0; dim < 3; dim++) {
            gsl::span<glm::vec3> inputRange(inputIter, inputEnd);
            OutputIteratorPolygonBuilder builder(outputIter);
            AxisAlignedPlane boundary = { dim, clipBox.minimum[dim] };
            boundary.splitConvexPolygon(inputRange, NullPolygonBuilder(), builder);

            std::swap(inputIter, outputIter);
            inputEnd = builder.getIterator();
        }

        // Now the max planes in each dimension
        for (uint8_t dim = 0; dim < 3; dim++) {
            gsl::span<glm::vec3> inputRange(inputIter, inputEnd);
            OutputIteratorPolygonBuilder builder(outputIter);
            AxisAlignedPlane boundary = { dim, clipBox.maximum[dim] };
            boundary.splitConvexPolygon(inputRange, builder, NullPolygonBuilder());

            std::swap(inputIter, outputIter);
            inputEnd = builder.getIterator();
        }

        // Finally split into left and right polygon via the split plane
        std::array<glm::vec3, 9> leftVertices;
        std::array<glm::vec3, 9> rightVertices;
        gsl::span<glm::vec3> inputRange(inputIter, inputEnd);

        OutputIteratorPolygonBuilder leftBuilder(leftVertices.begin());
        OutputIteratorPolygonBuilder rightBuilder(rightVertices.begin());
        splitPlane.splitConvexPolygon(inputRange, leftBuilder, rightBuilder);

        gsl::span<glm::vec3> leftRange(leftVertices.begin(), leftBuilder.getIterator());
        gsl::span<glm::vec3> rightRange(rightVertices.begin(), rightBuilder.getIterator());
        return std::array<AxisAlignedBoundingBox, 2>{
            smallestBoxContainingVertices(leftRange),
            smallestBoxContainingVertices(rightRange),
        };
    }
};

template <typename SurfaceData>
struct LeafNode {
    std::unordered_set<Triangle<SurfaceData>*> triangles;
};

template <typename SurfaceData>
struct BranchNode;

template <typename SurfaceData>
using MapNode = std::variant<LeafNode<SurfaceData>, BranchNode<SurfaceData>>;

template <typename SurfaceData>
struct BranchNode {
    AxisAlignedPlane split;

    std::unique_ptr<MapNode<SurfaceData>> left;
    std::unique_ptr<MapNode<SurfaceData>> right;
};

template <typename SurfaceData>
class KDTree {
public:
    KDTree(std::vector<std::unique_ptr<Triangle<SurfaceData>>> triangles,
        std::unique_ptr<MapNode<SurfaceData>> root, const AxisAlignedBoundingBox boundingBox)
        : _triangles(std::move(triangles))
        , _root(std::move(root))
        , _boundingBox(boundingBox)
    {
    }

    void dump() const
    {
        std::cout << "[KDTree]{" << std::endl;
        printBoundingBox(_boundingBox, 2);
        std::visit([this](const auto& root) { printNode(_boundingBox, root, 2); }, *_root);
        std::cout << "}" << std::endl;
    }

private:
    void printIndent(size_t indent = 0) const
    {
        for (size_t i = 0; i < indent; i++) {
            std::cout << " ";
        }
    }

    void printVertex(const glm::vec3& vertex) const
    {
        std::cout << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")";
    }

    void printBoundingBox(const AxisAlignedBoundingBox& box, size_t indent = 0) const
    {
        printIndent(indent);
        std::cout << "[Box]{ Min: ";
        printVertex(box.minimum);
        std::cout << " Max: ";
        printVertex(box.maximum);
        std::cout << " }" << std::endl;
    }

    void printTriangle(const Triangle<SurfaceData>& triangle, size_t indent = 0) const
    {
        printIndent(indent);
        std::cout << "[Triangle]{" << std::endl;
        for (const auto& vertex : triangle.vertices) {
            printIndent(indent + 2);
            printVertex(vertex);
            std::cout << std::endl;
        }
        printIndent(indent);
        std::cout << "}" << std::endl;
    }

    void printNode(const AxisAlignedBoundingBox& box, const LeafNode<SurfaceData>& node,
        size_t indent = 0) const
    {
        printIndent(indent);
        std::cout << "[Leaf]{" << std::endl;
        printBoundingBox(box, indent + 2);
        for (const auto& triangle : node.triangles) {
            printTriangle(*triangle, indent + 2);
        }
        printIndent(indent);
        std::cout << "}" << std::endl;
    }

    void printNode(const AxisAlignedBoundingBox& box, const BranchNode<SurfaceData>& node,
        size_t indent = 0) const
    {
        printIndent(indent);
        std::cout << "[Branch]{" << std::endl;
        printBoundingBox(box, indent + 2);
        auto [leftBox, rightBox] = box.split(node.split);
        std::visit([this, indent, leftBox = leftBox](
                       const auto& childNode) { printNode(leftBox, childNode, indent + 2); },
            *node.left);
        std::visit([this, indent, rightBox = rightBox](
                       const auto& childNode) { printNode(rightBox, childNode, indent + 2); },
            *node.right);
        printIndent(indent);
        std::cout << "}" << std::endl;
    }

    std::vector<std::unique_ptr<Triangle<SurfaceData>>> _triangles;
    std::unique_ptr<MapNode<SurfaceData>> _root;
    AxisAlignedBoundingBox _boundingBox;
};

template <typename SurfaceData>
class KDTreeBuilder {
public:
    void addTriangle(std::array<glm::vec3, 3> vertices, SurfaceData data)
    {
        auto triangle
            = std::make_unique<Triangle<SurfaceData>>(Triangle<SurfaceData>{ vertices, data });
        auto boundingBox = smallestBoxContainingVertices(vertices);
        _boundingBox.expandToBox(boundingBox);

        for (const auto& event : buildTriangleEvents(boundingBox, triangle.get())) {
            _events.insert(event);
        }
        _triangles.push_back(std::move(triangle));
    }

    KDTree<SurfaceData> build()
    {
        Expects(!_triangles.empty());
        Expects(!_events.empty());

        std::unordered_set<Triangle<SurfaceData>*> triangleSet;
        for (const auto& triangle : _triangles) {
            triangleSet.insert(triangle.get());
        }
        auto rootNode = createNode(_boundingBox, std::move(_events), triangleSet);
        return KDTree<SurfaceData>{ std::move(_triangles), std::move(rootNode), _boundingBox };
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

        bool operator<(const Event& other) const
        {
            auto tuplify = [](const Event& event) {
                return std::tie(event.separationPlane.boundary,
                    event.separationPlane.dimensionIndex, event.type, event.triangle);
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
        size_t triangleCount = triangles.size();
        auto [plane, side, cost] = findBestSplit(box, events, triangleCount);
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
                        triangles.erase(event.triangle);
                        leftTriangles.insert(event.triangle);
                    }
                } else if (eventPlane.boundary > plane.boundary) {
                    if (event.type != Event::Type::Ending) {
                        triangles.erase(event.triangle);
                        rightTriangles.insert(event.triangle);
                    }
                } else {
                    // On the boundary
                    triangles.erase(event.triangle);
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
                if (leftTriangles.count(event.triangle)) {
                    leftEvents.insert(event);
                } else if (rightTriangles.count(event.triangle)) {
                    rightEvents.insert(event);
                }
            }
            events.clear();

            // Overlapping triangles remain
            for (const auto& triangle : triangles) {
                auto [leftBox, rightBox] = triangle->clipAndSplitBoundingBoxes(box, plane);
                for (const auto& event : buildTriangleEvents(leftBox, triangle)) {
                    leftEvents.insert(event);
                }
                for (const auto& event : buildTriangleEvents(rightBox, triangle)) {
                    rightEvents.insert(event);
                }
                leftTriangles.insert(triangle);
                rightTriangles.insert(triangle);
            }
            triangles.clear();

            auto [leftBox, rightBox] = box.split(plane);
            return std::make_unique<MapNode<SurfaceData>>(BranchNode<SurfaceData>{
                plane,
                createNode(leftBox, std::move(leftEvents), std::move(leftTriangles)),
                createNode(rightBox, std::move(rightEvents), std::move(rightTriangles)),
            });
        } else {
            return std::make_unique<MapNode<SurfaceData>>(
                LeafNode<SurfaceData>{ std::move(triangles) });
        }
    }

    std::set<Event> buildTriangleEvents(
        const AxisAlignedBoundingBox& boundingBox, Triangle<SurfaceData>* triangle)
    {
        AxisAlignedBoundingBox actualBox = triangle->getBoundingBox();
        std::set<Event> events;
        for (uint8_t k = 0; k < 3; k++) {
            float minimum = boundingBox.minimum[k];
            float maximum = boundingBox.maximum[k];
            Expects(!(minimum < actualBox.minimum[k]));
            Expects(!(maximum > actualBox.maximum[k]));
            if (minimum < maximum) {
                events.insert(
                    Event{ triangle, AxisAlignedPlane{ k, maximum }, Event::Type::Ending });
                events.insert(
                    Event{ triangle, AxisAlignedPlane{ k, minimum }, Event::Type::Starting });
            } else {
                events.insert(
                    Event{ triangle, AxisAlignedPlane{ k, minimum }, Event::Type::Planar });
            }
        }
        return events;
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
                    && !(iter->separationPlane.boundary > candidatePlane.boundary)
                    && (iter->separationPlane.dimensionIndex == dimension);
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
        auto [leftBox, rightBox] = box.split(splitPlane);
        float boxSurfaceArea = box.getSurfaceArea();

        float leftArea = leftBox.getSurfaceArea() / boxSurfaceArea;
        float rightArea = rightBox.getSurfaceArea() / boxSurfaceArea;
        bool removesVolume = (leftBox.getVolume() > 0.0f) && (rightBox.getVolume() > 0.0f);

        auto cost = [leftArea, rightArea, removesVolume](
                        size_t leftTriangleCount, size_t rightTriangleCount) {
            float cost = static_cast<float>(leftTriangleCount) * leftArea
                + static_cast<float>(rightTriangleCount) * rightArea;
            if (removesVolume && (!leftTriangleCount || !rightTriangleCount)) {
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