#include "rev/geometry/Tools.h"

#include <gtest/gtest.h>

using namespace rev;

namespace {
void compareVertices(const glm::vec3& left, const glm::vec3& right)
{
    for (int i = 0; i < 3; i++) {
        EXPECT_FLOAT_EQ(left[i], right[i]);
    }
}

template <typename LeftVertexRange, typename RightVertexRange>
void comparePolygons(const LeftVertexRange& leftPolygon, const RightVertexRange& rightPolygon)
{
    ASSERT_EQ(leftPolygon.size(), rightPolygon.size());
    auto leftIter = std::begin(leftPolygon);
    auto rightIter = std::begin(rightPolygon);
    auto leftEnd = std::end(leftPolygon);
    while (leftIter != leftEnd) {
        compareVertices(*leftIter, *rightIter);
        leftIter++;
        rightIter++;
    }
}
}

TEST(GeometryTools, AxisAlignedPlaneWithAlignedSegmentIntersectionTest)
{
    Segment segment{ glm::vec3(-10.0f, 3.0f, 4.0f), glm::vec3(20.0f, 3.0f, 4.0f) };
    AxisAlignedPlane plane{ 0, 5.0f };
    glm::vec3 intersection = plane.intersect(segment);

    compareVertices(intersection, glm::vec3(5.0f, 3.0f, 4.0f));
}

TEST(GeometryTools, AxisAlignedPlaneWithNonAlignedSegmentIntersectionTest)
{
    Segment segment{ glm::vec3(0.0f, 3.0f, 4.0f), glm::vec3(10.0f, 19.0f, -1.0f) };
    AxisAlignedPlane plane{ 0, 7.5f };
    glm::vec3 intersection = plane.intersect(segment);

    compareVertices(intersection, glm::vec3(7.5f, 15.0f, 0.25f));
}

TEST(GeometryTools, NonIntersectingPolygonNotSplitByPlane)
{
    std::array<glm::vec3, 3> triangle{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(3.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 4.0f, 0.0f),
    };

    AxisAlignedPlane plane{ 0, 5.0f };

    std::vector<glm::vec3> leftVertices;
    std::vector<glm::vec3> rightVertices;

    plane.splitConvexPolygon(triangle,
        OutputIteratorPolygonBuilder(std::back_inserter(leftVertices)),
        OutputIteratorPolygonBuilder(std::back_inserter(rightVertices)));

    comparePolygons(leftVertices, triangle);
    comparePolygons(rightVertices, gsl::span<glm::vec3>());
}

TEST(GeometryTools, PolygonWithCoplanarSegmentNotSplitByPlane)
{
    std::array<glm::vec3, 3> triangle{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(5.0f, 3.0f, 0.0f),
        glm::vec3(5.0f, 0.0f, 0.0f),
    };

    AxisAlignedPlane plane{ 0, 5.0f };

    std::vector<glm::vec3> leftVertices;
    std::vector<glm::vec3> rightVertices;

    plane.splitConvexPolygon(triangle,
        OutputIteratorPolygonBuilder(std::back_inserter(leftVertices)),
        OutputIteratorPolygonBuilder(std::back_inserter(rightVertices)));

    comparePolygons(leftVertices, triangle);
    comparePolygons(rightVertices,
        std::initializer_list<glm::vec3>{
            glm::vec3(5.0f, 3.0f, 0.0f),
            glm::vec3(5.0f, 0.0f, 0.0f),
        });
}

TEST(GeometryTools, SplitPolygonIntersectedByPlane)
{
    std::array<glm::vec3, 3> triangle{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(3.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 4.0f, 0.0f),
    };

    AxisAlignedPlane plane{ 0, 1.5f };

    std::vector<glm::vec3> leftVertices;
    std::vector<glm::vec3> rightVertices;

    plane.splitConvexPolygon(triangle,
        OutputIteratorPolygonBuilder(std::back_inserter(leftVertices)),
        OutputIteratorPolygonBuilder(std::back_inserter(rightVertices)));

    comparePolygons(leftVertices,
        std::initializer_list<glm::vec3>{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.5f, 0.0f, 0.0f),
            glm::vec3(1.5f, 2.0f, 0.0f),
            glm::vec3(0.0f, 4.0f, 0.0f),
        });
    comparePolygons(rightVertices,
        std::initializer_list<glm::vec3>{
            glm::vec3(1.5f, 0.0f, 0.0f),
            glm::vec3(3.0f, 0.0f, 0.0f),
            glm::vec3(1.5f, 2.0f, 0.0f),
        });
}

TEST(GeometryTools, NullPolygonBuilder)
{
    std::array<glm::vec3, 3> triangle{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 4.0f),
    };

    AxisAlignedPlane plane{ 0, 2.0f };
    plane.splitConvexPolygon(triangle, NullPolygonBuilder{}, NullPolygonBuilder{});
}
