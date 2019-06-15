#include "rev/geometry/KDTree.h"

#include <gtest/gtest.h>

using namespace rev;

namespace {
struct TestSurfaceData {
    uint64_t id;
};
}

TEST(KDTreeTests, BuildEmptyTree)
{
    KDTreeBuilder<TestSurfaceData> builder;
    auto tree = builder.build();
}