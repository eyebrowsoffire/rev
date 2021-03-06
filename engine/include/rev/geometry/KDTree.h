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

    glm::vec3 getNormal() const
    {
        auto e1 = vertices[1] - vertices[0];
        auto e2 = vertices[2] - vertices[0];
        return glm::normalize(glm::cross(e1, e2));
    }

    bool isLeftOfPlane(const AxisAlignedPlane& plane) const
    {
        for (const auto& vertex : vertices) {
            if (vertex[plane.dimensionIndex] < plane.boundary) {
                return true;
            }
        }
        return false;
    }

    bool isRightOfPlane(const AxisAlignedPlane& plane) const
    {
        for (const auto& vertex : vertices) {
            if (vertex[plane.dimensionIndex] > plane.boundary) {
                return true;
            }
        }
        return false;
    }

    std::optional<AxisAlignedBoundingBox> clippedBoundingBox(const AxisAlignedBoundingBox& box)
    {
        // Alternate between these two buffers for input and output
        std::array<glm::vec3, 9> buf1{};
        std::array<glm::vec3, 9> buf2{};

        // Fill the input buffer
        for (int i = 0; i < 3; i++) {
            buf1[i] = vertices[i];
        }

        // Clip the triangle by all sides of our bounding box
        auto inputIter = buf1.data();
        auto inputEnd = inputIter + 3;
        auto outputIter = buf2.data();

        // First the min planes in each dimension
        for (uint8_t dim = 0; dim < 3; dim++) {
            gsl::span<glm::vec3> inputRange(inputIter, inputEnd);
            OutputIteratorPolygonBuilder builder(outputIter);
            AxisAlignedPlane boundary = { dim, box.minimum[dim] };
            boundary.splitConvexPolygon(inputRange, NullPolygonBuilder(), builder);

            if (std::distance(outputIter, builder.getIterator()) < 3) {
                return {};
            }

            std::swap(inputIter, outputIter);
            inputEnd = builder.getIterator();
        }

        // Now the max planes in each dimension
        for (uint8_t dim = 0; dim < 3; dim++) {
            gsl::span<glm::vec3> inputRange(inputIter, inputEnd);
            OutputIteratorPolygonBuilder builder(outputIter);
            AxisAlignedPlane boundary = { dim, box.maximum[dim] };
            boundary.splitConvexPolygon(inputRange, builder, NullPolygonBuilder());

            if (std::distance(outputIter, builder.getIterator()) < 3) {
                return {};
            }

            std::swap(inputIter, outputIter);
            inputEnd = builder.getIterator();
        }

        gsl::span<glm::vec3> inputRange(inputIter, inputEnd);
        auto clippedBox = smallestBoxContainingVertices(inputRange);
        clippedBox.reduceToBox(box);
        return clippedBox;
    }

    struct Hit {
        glm::vec2 uv;
        float t;
    };

    std::optional<Hit> castRay(const Ray& ray) const
    {
        glm::vec3 edge1 = vertices[1] - vertices[0];
        glm::vec3 edge2 = vertices[2] - vertices[0];

        glm::vec3 p = glm::cross(ray.direction, edge2);
        float determinant = glm::dot(edge1, p);
        if (abs(determinant) < std::numeric_limits<float>::epsilon()) {
            // The ray is parallel.
            return std::nullopt;
        }

        glm::vec3 fromV0 = ray.origin - vertices[0];
        float u = glm::dot(fromV0, p) / determinant;
        if (u < 0.0f || u > 1.0f) {
            return std::nullopt;
        }

        glm::vec3 q = glm::cross(fromV0, edge1);
        float v = glm::dot(ray.direction, q) / determinant;
        if (v < 0.0f || v > 1.0f) {
            return std::nullopt;
        }

        float t = glm::dot(edge2, q) / determinant;
        if (t < 0.0f) {
            return std::nullopt;
        }

        return Hit{ { u, v }, t };
    }

    glm::vec3 baryCentricToCartesian(glm::vec2 uv)
    {
        float u = uv[0];
        float v = uv[1];
        float w = 1.0f - (u + v);
        return w * vertices[0] + u * vertices[1] + v * vertices[2];
    }

    glm::vec2 cartesianToBarycentric(glm::vec3 position)
    {
        glm::vec3 v0 = vertices[1] - vertices[0];
        glm::vec3 v1 = vertices[2] - vertices[0];
        glm::vec3 v2 = position - vertices[0];

        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float denom = d00 * d11 - d01 * d01;

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        return { u, v };
    }

    std::optional<float> intersectsSphere(const Sphere& sphere)
    {
        glm::vec3 normal = getNormal();
        float t = glm::dot(normal, (vertices[0] - sphere.center));
        if (sphere.radius < abs(t)) {
            // Closest point to the triangle is outside the sphere.
            return std::nullopt;
        }

        glm::vec3 closestPoint = sphere.center - (normal * t);
        glm::vec2 cpBary = cartesianToBarycentric(closestPoint);
        size_t outsideCount = 0;
        glm::bvec3 isOutside{};
        if (cpBary[0] < 0.0f) {
            outsideCount++;
            isOutside[0] = true;
        }

        if (cpBary[1] < 0.0f) {
            outsideCount++;
            isOutside[1] = true;
        }

        if ((1.0f - (cpBary[0] + cpBary[1])) < 0.0f) {
            outsideCount++;
            isOutside[2] = true;
        }

        if (outsideCount == 0) {
            // Closest point is inside the triangle and inside the sphere.
            return t;
        }

        // Calculate the square of the radius of the cross section of the sphere passing through
        // the triangle's plane.
        float r2 = (sphere.radius * sphere.radius) - (t * t);

        if (outsideCount == 1) {
            // We need to find the distance to the edge (squared) here.
            glm::vec3 edge;
            glm::vec3 toVertex;
            if (isOutside[0]) {
                edge = vertices[2] - vertices[1];
                toVertex = closestPoint - vertices[1];
            } else if (isOutside[1]) {
                edge = vertices[2] - vertices[0];
                toVertex = closestPoint - vertices[0];
            } else {
                edge = vertices[1] - vertices[0];
                toVertex = closestPoint - vertices[0];
            }

            float toVertexLength = glm::length(toVertex);
            float legLength = glm::dot(edge, toVertex) / toVertexLength;

            return ((toVertexLength * toVertexLength) - (legLength * legLength)) < r2;
        }

        // Otherwise, one of the vertices is actually the closest point on the triangle.
        glm::vec3 toVertex;
        if (!isOutside[0]) {
            toVertex = closestPoint - vertices[0];
        } else if (!isOutside[1]) {
            toVertex = closestPoint - vertices[1];
        } else {
            toVertex = closestPoint - vertices[2];
        }

        return glm::dot(toVertex, toVertex) < r2;
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

    struct Hit {
        Triangle<SurfaceData>* triangle;
        glm::vec2 uv;
        float t;
    };

    std::optional<Hit> castRay(
        const Ray& ray, float maxDistance = std::numeric_limits<float>::infinity()) const
    {
        if (_boundingBox.containsPoint(ray.origin)) {
            AxisAlignedBoundingBox::Hit entryPoint{ ray.origin, 0.0f, 0 };
            return getHit(*_root, ray, _boundingBox, entryPoint, maxDistance);
        } else {
            auto hit = _boundingBox.castExternalRay(ray);
            if (!hit || (hit->t > maxDistance)) {
                return std::nullopt;
            }
            return getHit(*_root, ray, _boundingBox, *hit, maxDistance);
        }
    }

    template <typename Visitor>
    void visitTrianglesIntersectingSphere(const Sphere& sphere, Visitor&& visitor) const
    {
        std::visit(
            [this, &sphere, &visitor](const auto& node) {
                visitTrianglesIntersectingSphereInNode(
                    sphere, std::forward<Visitor>(visitor), _boundingBox, node);
            },
            *_root);
    }

    void dump() const
    {
        std::cout << "[KDTree]{" << std::endl;
        printBoundingBox(_boundingBox, 2);
        std::visit([this](const auto& root) { printNode(_boundingBox, root, 2); }, *_root);
        std::cout << "}" << std::endl;
    }

private:
    std::optional<Hit> getHit(const MapNode<SurfaceData>& node, const Ray& ray,
        const AxisAlignedBoundingBox& box, AxisAlignedBoundingBox::Hit& entryPoint,
        float maxDistance) const
    {
        return std::visit(
            [this, &ray, &box, &entryPoint, maxDistance](
                const auto& node) { return getHit(node, ray, box, entryPoint, maxDistance); },
            node);
    }

    std::optional<Hit> getHit(const BranchNode<SurfaceData>& node, const Ray& ray,
        const AxisAlignedBoundingBox& box, AxisAlignedBoundingBox::Hit& entryPoint,
        float maxDistance) const
    {
        auto [leftBox, rightBox] = box.split(node.split);
        float position = entryPoint.intersectionPoint[node.split.dimensionIndex];
        bool goLeft;
        if (position < node.split.boundary) {
            goLeft = true;
        } else if (position > node.split.boundary) {
            goLeft = false;
        } else if (ray.direction[node.split.dimensionIndex] < 0.0f) {
            goLeft = false;
        } else if (ray.direction[node.split.dimensionIndex] > 0.0f) {
            goLeft = true;
        } else {
            return std::nullopt;
        }

        MapNode<SurfaceData>* childNode = goLeft ? node.left.get() : node.right.get();
        auto hit = getHit(*childNode, ray, (goLeft ? leftBox : rightBox), entryPoint, maxDistance);
        if (hit) {
            return hit;
        }
        if (entryPoint.t > maxDistance) {
            return std::nullopt;
        }
        if (entryPoint.planeDimension != node.split.dimensionIndex) {
            return std::nullopt;
        }

        bool shouldTryOtherChild;
        if (goLeft) {
            shouldTryOtherChild = (ray.direction[node.split.dimensionIndex] > 0.0f);
        } else {
            shouldTryOtherChild = (ray.direction[node.split.dimensionIndex] < 0.0f);
        }

        if (shouldTryOtherChild) {
            childNode = goLeft ? node.right.get() : node.left.get();
            return getHit(*childNode, ray, (goLeft ? rightBox : leftBox), entryPoint, maxDistance);
        }
        return std::nullopt;
    }

    std::optional<Hit> getHit(const LeafNode<SurfaceData>& node, const Ray& ray,
        const AxisAlignedBoundingBox& box, AxisAlignedBoundingBox::Hit& entryPoint,
        float maxDistance) const
    {
        float t = std::numeric_limits<float>::infinity();
        std::optional<Hit> bestHit;
        for (const auto& triangle : node.triangles) {
            auto hit = triangle->castRay(ray);
            if (hit) {
                if (!bestHit || (hit->t < bestHit->t)) {
                    bestHit = Hit{ triangle, hit->uv, hit->t };
                }
            }
        }

        if (bestHit && !(bestHit->t > maxDistance)) {
            return bestHit;
        }

        auto boxHit = box.castInternalRay(ray);
        entryPoint = boxHit;

        return std::nullopt;
    }

    template <typename Visitor>
    void visitTrianglesIntersectingSphereInNode(const Sphere& sphere, Visitor&& visitor,
        const AxisAlignedBoundingBox& box, const LeafNode<SurfaceData>& node) const
    {
        if (!box.intersectsSphere(sphere)) {
            return;
        }

        for (const auto& triangle : node.triangles) {
            auto hit = triangle->intersectsSphere(sphere);
            if (hit) {
                visitor(*triangle, *hit);
            }
        }
    }

    template <typename Visitor>
    void visitTrianglesIntersectingSphereInNode(const Sphere& sphere, Visitor&& visitor,
        const AxisAlignedBoundingBox& box, const BranchNode<SurfaceData>& node) const
    {
        if (!box.intersectsSphere(sphere)) {
            return;
        }

        auto [leftBox, rightBox] = box.split(node.split);
        std::visit(
            [this, leftBox = leftBox, &sphere, &visitor](const auto& node) {
                visitTrianglesIntersectingSphereInNode(
                    sphere, std::forward<Visitor>(visitor), leftBox, node);
            },
            *node.left);
        std::visit(
            [this, rightBox = rightBox, &sphere, &visitor](const auto& node) {
                visitTrianglesIntersectingSphereInNode(
                    sphere, std::forward<Visitor>(visitor), rightBox, node);
            },
            *node.right);
    }

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
        Expects(!glm::any(glm::isnan(vertices[0])));
        Expects(!glm::any(glm::isnan(vertices[1])));
        Expects(!glm::any(glm::isnan(vertices[2])));

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
        if (triangleCount > 0) {
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
                            size_t erased = triangles.erase(event.triangle);
                            Expects(erased == 1);
                            leftTriangles.insert(event.triangle);

                            Expects(event.triangle->isLeftOfPlane(plane));
                        }
                    } else if (eventPlane.boundary > plane.boundary) {
                        if (event.type != Event::Type::Ending) {
                            size_t erased = triangles.erase(event.triangle);
                            Expects(erased == 1);
                            rightTriangles.insert(event.triangle);

                            Expects(event.triangle->isRightOfPlane(plane));
                        }
                    } else {
                        // On the boundary
                        size_t erased = triangles.erase(event.triangle);
                        Expects(erased == 1);
                        switch (event.type) {
                        case Event::Type::Starting:
                            rightTriangles.insert(event.triangle);
                            Expects(event.triangle->isRightOfPlane(plane));
                            break;
                        case Event::Type::Ending:
                            leftTriangles.insert(event.triangle);
                            Expects(event.triangle->isLeftOfPlane(plane));
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

                auto [leftBox, rightBox] = box.split(plane);
                // Overlapping triangles remain
                for (const auto& triangle : triangles) {
                    Expects(triangle->isLeftOfPlane(plane));
                    Expects(triangle->isRightOfPlane(plane));

                    auto leftClipBox = triangle->clippedBoundingBox(leftBox);
                    if (leftClipBox) {
                        for (const auto& event : buildTriangleEvents(*leftClipBox, triangle))
                        {
                            leftEvents.insert(event);
                        }
                        leftTriangles.insert(triangle);
                    }

                    auto rightClipBox = triangle->clippedBoundingBox(rightBox);
                    if (rightClipBox) {
                        for (const auto& event : buildTriangleEvents(*rightClipBox, triangle))
                        {
                            rightEvents.insert(event);
                        }
                        rightTriangles.insert(triangle);
                    }
                }
                triangles.clear();
                events.clear();

                return std::make_unique<MapNode<SurfaceData>>(BranchNode<SurfaceData>{
                    plane,
                    createNode(leftBox, std::move(leftEvents), std::move(leftTriangles)),
                    createNode(rightBox, std::move(rightEvents), std::move(rightTriangles)),
                });
            }
        }
        return std::make_unique<MapNode<SurfaceData>>(
            LeafNode<SurfaceData>{ std::move(triangles) });
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
        AxisAlignedPlane bestPlane;
        Side bestSide;

        std::array<size_t, 3> leftTriangleCounts{ 0 };
        std::array<size_t, 3> rightTriangleCounts{ triangleCount, triangleCount, triangleCount };
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
                bestPlane = candidatePlane;
                bestSide = side;
            }
            leftTriangleCounts[dimension] += planarTriangleCount;
            leftTriangleCounts[dimension] += startingTriangleCount;
        }
        return { bestPlane, bestSide, bestCost };
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