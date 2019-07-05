#include "rev/NurbsCurve.h"

#include <cmath>
#include <gtest/gtest.h>
#include <iostream>

using namespace rev;

TEST(NurbsCurveTests, TestNurbsCircle)
{
    float halfRoot2 = sqrt(2.0f) / 2.0f;
    WeightedControlPoint<glm::vec2> controlPoints[] = {
        { { 1.0, 0.0 }, 1.0 },
        { { 1.0, 1.0 }, halfRoot2 },
        { { 0.0, 1.0 }, 1.0 },
        { { -1.0, 1.0 }, halfRoot2 },
        { { -1.0, 0.0 }, 1.0 },
        { { -1.0, -1.0 }, halfRoot2 },
        { { 0.0, -1.0 }, 1.0 },
        { { 1.0, -1.0 }, halfRoot2 },
        { { 1.0, 0.0 }, 1.0 },
    };
    constexpr float halfPi = 3.141592653589793846f / 2.0f;
    float knots[] = {
        0.0f,
        0.0f,
        0.0f,
        halfPi,
        halfPi,
        halfPi * 2.0f,
        halfPi * 2.0f,
        halfPi * 3.0f,
        halfPi * 3.0f,
        halfPi * 4.0f,
        halfPi * 4.0f,
        halfPi * 4.0f,
    };

    NurbsCurve<glm::vec2> curve(3, knots, controlPoints);

    for (float i = 0.0f; i < halfPi * 4.0f; i += 0.01f) {
        glm::vec2 point = curve[i];
        ASSERT_FLOAT_EQ(glm::dot(point, point), 1.0f);
    }
}
