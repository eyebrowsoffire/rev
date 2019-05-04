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
    constexpr float halfPi = 3.141592653589793846 / 2.0;
    float knots[] = {
        0.0,
        0.0,
        0.0,
        halfPi,
        halfPi,
        halfPi * 2.0,
        halfPi * 2.0,
        halfPi * 3.0,
        halfPi * 3.0,
        halfPi * 4.0,
        halfPi * 4.0,
        halfPi * 4.0,
    };

    NurbsCurve<glm::vec2> curve(3, knots, controlPoints);

    for (float i = 0.0; i < halfPi * 4.0; i += 0.01) {
        glm::vec2 point = curve[i];
        ASSERT_FLOAT_EQ(glm::dot(point, point), 1.0);
    }
}
