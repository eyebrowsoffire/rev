#include "rev/Unit.h"

#include <glm/glm.hpp>
#include <gtest/gtest.h>

using namespace rev;

namespace
{
    struct TestComponent1
    {
    };

    struct TestComponent2
    {
    };

    struct TestComponent3
    {
    };

    using TestComponentEnumeration = UnitComponentEnumeration<TestComponent1, TestComponent2, TestComponent3>;

    using TestUnitComp1 = TestComponentEnumeration::MakeComposition<TestComponent1>;
    using TestUnitComp2 = TestComponentEnumeration::MakeComposition<TestComponent2>;
    using TestUnitComp3 = TestComponentEnumeration::MakeComposition<TestComponent3>;

    using TestUnitComp1x1 = MultiplyComposition<TestUnitComp1, TestUnitComp1>;
    using TestUnitComp1x2 = MultiplyComposition<TestUnitComp1, TestUnitComp2>;
    using TestUnitComp3_1x1 = MultiplyComposition<TestUnitComp3, InvertComposition<TestUnitComp1x1>>;

    static_assert(std::is_same_v<TestUnitComp3, MultiplyComposition<TestUnitComp3_1x1, TestUnitComp1x1>>);

    template <typename ValueType>
    using TestUnit1 = Unit<ValueType, TestUnitComp1>;

    template <typename ValueType>
    using TestUnit2 = Unit<ValueType, TestUnitComp2>;

    template <typename ValueType>
    using TestUnit3 = Unit<ValueType, TestUnitComp3>;

    template <typename ValueType>
    using TestUnit1x1 = Unit<ValueType, TestUnitComp1x1>;

    template <typename ValueType>
    using TestUnit1x2 = Unit<ValueType, TestUnitComp1x2>;

    template <typename ValueType>
    using TestUnit3_1x1 = Unit<ValueType, TestUnitComp3_1x1>;
}

TEST(UnitUnitTests, TestCreateFloatUnits)
{
    TestUnit1<float> unit1;
    TestUnit2<float> unit2;
    TestUnit3<float> unit3;
    TestUnit1x1<float> unit1x1;
    TestUnit1x2<float> unit1x2;
    TestUnit3_1x1<float> unit3_1x1;
}

TEST(UnitUnitTests, TestCreateVecUnits)
{
    TestUnit1<glm::vec3> vecUnit1;
    TestUnit2<glm::vec3> vecUnit2;
    TestUnit3<glm::vec3> vecUnit3;
    TestUnit1x1<glm::vec3> vecUnit1x1;
    TestUnit1x2<glm::vec3> vecUnit1x2;
    TestUnit3_1x1<glm::vec3> vecUnit3_1x1;
}

TEST(UnitUnitTests, TestAddFloatUnits)
{
    TestUnit1<float> floatUnit1(1.5f);
    TestUnit1<float> floatUnit2(1.8f);

    TestUnit1<float> sum = floatUnit1 + floatUnit2;

    ASSERT_FLOAT_EQ(sum.getValue(), 3.3f);

    sum += floatUnit1;

    ASSERT_FLOAT_EQ(sum.getValue(), 4.8f);
}

TEST(UnitUnitTests, TestAddVecUnits)
{
    TestUnit1<glm::vec3> vecUnit1(1.5f, 0.7f, 4.5f);
    TestUnit1<glm::vec3> vecUnit2(1.8f, 1.1f, 6.7f);

    TestUnit1<glm::vec3> sum = vecUnit1 + vecUnit2;

    ASSERT_FLOAT_EQ(sum.getValue()[0], 3.3f);
    ASSERT_FLOAT_EQ(sum.getValue()[1], 1.8f);
    ASSERT_FLOAT_EQ(sum.getValue()[2], 11.2f);

    sum += vecUnit1;

    ASSERT_FLOAT_EQ(sum.getValue()[0], 4.8f);
    ASSERT_FLOAT_EQ(sum.getValue()[1], 2.5f);
    ASSERT_FLOAT_EQ(sum.getValue()[2], 15.7f);
}

TEST(UnitUnitTests, TestSubtractFloatUnits)
{
    TestUnit1<float> floatUnit1(1.8f);
    TestUnit1<float> floatUnit2(1.5f);

    TestUnit1<float> diff = floatUnit1 - floatUnit2;

    ASSERT_FLOAT_EQ(diff.getValue(), 0.3f);

    diff -= floatUnit1;

    ASSERT_FLOAT_EQ(diff.getValue(), -1.5f);
}

TEST(UnitUnitTests, TestSubtractVecUnits)
{
    TestUnit1<glm::vec3> vecUnit1(1.8f, 1.1f, 6.7f);
    TestUnit1<glm::vec3> vecUnit2(1.5f, 0.7f, 4.5f);

    TestUnit1<glm::vec3> diff = vecUnit1 - vecUnit2;

    ASSERT_FLOAT_EQ(diff.getValue()[0], 0.3f);
    ASSERT_FLOAT_EQ(diff.getValue()[1], 0.4f);
    ASSERT_FLOAT_EQ(diff.getValue()[2], 2.2f);

    diff -= vecUnit1;

    ASSERT_FLOAT_EQ(diff.getValue()[0], -1.5f);
    ASSERT_FLOAT_EQ(diff.getValue()[1], -0.7f);
    ASSERT_FLOAT_EQ(diff.getValue()[2], -4.5f);
}

TEST(UnitUnitTests, TestMultiplyFloatUnits)
{
    TestUnit1<float> firstUnit1(1.1f);
    TestUnit1<float> secondUnit1(2.0f);

    TestUnit1x1<float> product1x1 = firstUnit1 * secondUnit1;

    ASSERT_FLOAT_EQ(product1x1.getValue(), 2.2f);

    TestUnit2<float> unit2(3.0f);
    
    TestUnit1x2<float> product1x2 = firstUnit1 * unit2;
    ASSERT_FLOAT_EQ(product1x2.getValue(), 3.3f);

    // Test commutivity
    product1x2 = unit2 * firstUnit1;
    ASSERT_FLOAT_EQ(product1x2.getValue(), 3.3f);
}

TEST(UnitUnitTests, TestDivideFloatUnits)
{
    TestUnit3<float> unit3(1.8f);
    TestUnit1x1<float> unit1x1(0.5f);

    TestUnit3_1x1<float> quotient3_1x1 = unit3 / unit1x1;
    
    ASSERT_FLOAT_EQ(quotient3_1x1.getValue(), 3.6f);

    TestUnit1<float> unit1(2.0f);

    TestUnit1<float> quotient1 = unit1x1 / unit1;

    ASSERT_FLOAT_EQ(quotient1.getValue(), 0.25f);
}

TEST(UnitUnitTests, TestMultiplyMixedUnits)
{
    TestUnit1<float> floatUnit1(1.5f);
    TestUnit1<glm::vec3> vecUnit1(3.6f, -1.2f, -0.8f);

    TestUnit1x1<glm::vec3> quotient1x1 = floatUnit1 * vecUnit1;

    ASSERT_FLOAT_EQ(quotient1x1.getValue()[0], 5.4f);
    ASSERT_FLOAT_EQ(quotient1x1.getValue()[1], -1.8f);
    ASSERT_FLOAT_EQ(quotient1x1.getValue()[2], -1.2f);

    TestUnit2<glm::vec3> vecUnit2(2.2f, 0.2f, 21.4f);

    TestUnit1x2<glm::vec3> quotient1x2 = floatUnit1 * vecUnit2;

    ASSERT_FLOAT_EQ(quotient1x2.getValue()[0], 3.3f);
    ASSERT_FLOAT_EQ(quotient1x2.getValue()[1], 0.3f);
    ASSERT_FLOAT_EQ(quotient1x2.getValue()[2], 32.1f);

    quotient1x2 = vecUnit2 * floatUnit1;

    ASSERT_FLOAT_EQ(quotient1x2.getValue()[0], 3.3f);
    ASSERT_FLOAT_EQ(quotient1x2.getValue()[1], 0.3f);
    ASSERT_FLOAT_EQ(quotient1x2.getValue()[2], 32.1f);
}

TEST(UnitUnitTests, TestDivideMixedUnits)
{
    TestUnit3<glm::vec3> unit3(1.8f, 1.1f, -17.3f);
    TestUnit1x1<float> unit1x1(0.5f);

    TestUnit3_1x1<glm::vec3> quotient3_1x1 = unit3 / unit1x1;
    
    ASSERT_FLOAT_EQ(quotient3_1x1.getValue()[0], 3.6f);
    ASSERT_FLOAT_EQ(quotient3_1x1.getValue()[1], 2.2f);
    ASSERT_FLOAT_EQ(quotient3_1x1.getValue()[2], -34.6f);

    TestUnit1<glm::vec3> unit1(2.0f, 4.0f, 5.0f);

    TestUnit1<glm::vec3> quotient1 = unit1x1 / unit1;

    ASSERT_FLOAT_EQ(quotient1.getValue()[0], 0.25f);
    ASSERT_FLOAT_EQ(quotient1.getValue()[1], 0.125f);
    ASSERT_FLOAT_EQ(quotient1.getValue()[2], 0.1f);
}
