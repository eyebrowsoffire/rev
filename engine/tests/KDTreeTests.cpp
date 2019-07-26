#include "rev/geometry/KDTree.h"

#include <gtest/gtest.h>

using namespace rev;

namespace {
struct TestSurfaceData {
    uint64_t id;
};
}

TEST(KDTreeTests, BuildTreeWithSomeTriangles)
{
    KDTreeBuilder<TestSurfaceData> builder;
    builder.addTriangle(
        {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 4.0f),
            glm::vec3(0.0f, 3.0f, 0.0f),
        },
        { 0 });
    builder.addTriangle(
        {
            glm::vec3(0.0f, 3.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 4.0f),
            glm::vec3(0.0f, 3.0f, 4.0f),
        },
        { 1 });

    builder.addTriangle(
        {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 4.0f),
            glm::vec3(0.0f, -3.0f, 0.0f),
        },
        { 2 });
    builder.addTriangle(
        {
            glm::vec3(0.0f, -3.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 4.0f),
            glm::vec3(0.0f, -3.0f, 4.0f),
        },
        { 3 });

    builder.addTriangle(
        {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 0.0f, 4.0f),
            glm::vec3(2.0f, -3.0f, 0.0f),
        },
        { 2 });
    builder.addTriangle(
        {
            glm::vec3(2.0f, -3.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 4.0f),
            glm::vec3(1.0f, -3.0f, 4.0f),
        },
        { 3 });

    auto tree = builder.build();
    Sphere sphere{glm::vec3{0.0f, 0.0f, 0.0f}, 4.0f};
    tree.visitTrianglesIntersectingSphere(sphere, [](Triangle<TestSurfaceData> &, float) {

    });
    tree.dump();
}