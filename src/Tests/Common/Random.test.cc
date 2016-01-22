// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Random.h"

namespace
{
    // dSFMT2-19937:117-19:ffafffffffb3f-ffdfffc90fffd
    // init_gen_rand(0) generated randoms [0, 1)
    const double kKnownRandoms[]{0.030581026769374, 0.213140320067012,
                                 0.299002525016001, 0.381138853044628,
                                 0.863488397063594, 0.133443440024236,
                                 0.073465290616508, 0.924735636004535};
}

TEST(RandomTest, InRange01)
{
    dsfmt_gv_init_gen_rand(0);

    for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
        ASSERT_FLOAT_EQ(kKnownRandoms[i], rainbow::random());
}

TEST(RandomTest, InRange0N)
{
    const double upper = 8;
    dsfmt_gv_init_gen_rand(0);

    for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
        ASSERT_FLOAT_EQ(kKnownRandoms[i] * upper, rainbow::random(upper));
}

TEST(RandomTest, InRangeMN)
{
    const double lower = 8.0;
    const double upper = 16.0;
    dsfmt_gv_init_gen_rand(0);

    for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
    {
        ASSERT_FLOAT_EQ(kKnownRandoms[i] * lower + lower,
                        rainbow::random(lower, upper));
    }
}

TEST(RandomTest, SeededInRange01)
{
    rainbow::random.seed(1);
    for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
    {
        ASSERT_FALSE(
            rainbow::is_equal<float>(rainbow::random(), kKnownRandoms[i]));
    }
}
