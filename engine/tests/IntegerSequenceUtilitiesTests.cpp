#include "rev/IntegerSequenceUtilities.h"

#include <gtest/gtest.h>

using namespace rev;

TEST(IntegerSequenceUtilitiesTests, SumIntegerSequences)
{
    using First = std::integer_sequence<size_t, 1, 2, 3, 4, 5>;
    using Second = std::integer_sequence<size_t, 5, 6, 7, 8, 9>;

    using ExpectedSum = std::integer_sequence<size_t, 6, 8, 10, 12, 14>;
    using ActualSum = SumIntegerSeq<First, Second>;

    static_assert(std::is_same_v<ActualSum, ExpectedSum>);
}

TEST(IntegerSequenceUtilitiesTests, MinIntegerSequences)
{
    using First = std::integer_sequence<size_t, 8, 2, 5, 0, 1>;
    using Second = std::integer_sequence<size_t, 5, 4, 7, 1, 1>;

    using ExpectedMin = std::integer_sequence<size_t, 5, 2, 5, 0, 1>;
    using ActualMin = MinIntegerSeq<First, Second>;

    static_assert(std::is_same_v<ActualMin, ExpectedMin>);
}

TEST(IntegerSequenceUtilitiesTests, SubtractIntegerSequences)
{
    using First = std::integer_sequence<size_t, 8, 5, 9, 3, 1>;
    using Second = std::integer_sequence<size_t, 5, 4, 7, 1, 1>;

    using ExpectedDiff = std::integer_sequence<size_t, 3, 1, 2, 2, 0>;
    using ActualDiff = SubtractIntegerSeq<First, Second>;

    static_assert(std::is_same_v<ActualDiff, ExpectedDiff>);
}