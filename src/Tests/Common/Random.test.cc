// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Random.h"

namespace
{
    constexpr unsigned int kSampleSize = 1u << 16;
    constexpr uint64_t kSeed = UINT64_C(1181783497276652981);
}

TEST(RandomTest, InRange01)
{
    rainbow::random.seed();

    for (unsigned int i = 0; i < kSampleSize; ++i)
    {
        const double v = rainbow::random();
        ASSERT_GE(v, 0.0);
        ASSERT_LT(v, 1.0);
    }
}

TEST(RandomTest, InRange0N)
{
    constexpr double upper = 6;
    rainbow::random.seed();

    for (unsigned int i = 0; i < kSampleSize; ++i)
    {
        const double v = rainbow::random(upper);
        ASSERT_GE(v, 0.0);
        ASSERT_LT(v, upper);
    }
}

TEST(RandomTest, InRangeMN)
{
    constexpr double lower = 1.0;
    constexpr double upper = 6.0;
    rainbow::random.seed();

    for (unsigned int i = 0; i < kSampleSize; ++i)
    {
        const double v = rainbow::random(lower, upper);
        ASSERT_GE(v, lower);
        ASSERT_LT(v, upper);
    }
}

TEST(RandomTest, Seed)
{
    rainbow::Random rand1;
    rand1.seed(kSeed);

    rainbow::Random rand2;
    rand2.seed(kSeed);

    for (unsigned int i = 0; i < kSampleSize; ++i)
        ASSERT_EQ(rand2(0.0, 1.0), rand1());

    rand2.seed(kSeed);

    ASSERT_NE(rand2(0.0, 1.0), rand1());

    rand1.seed(kSeed);
    rand2.seed(kSeed);

    ASSERT_EQ(rand2(0.0, 1.0), rand1());

    rand1.seed();
    rand2.seed();

    ASSERT_NE(rand2(0.0, 1.0), rand1());
}

TEST(RandomTest, SeededInRange01)
{
    rainbow::random.seed(kSeed);

    for (unsigned int i = 0; i < kSampleSize; ++i)
    {
        const double v = rainbow::random();
        ASSERT_GE(v, 0.0);
        ASSERT_LT(v, 1.0);
    }
}
