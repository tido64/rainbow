// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Math/Vec3.h"

using rainbow::Vec3f;

TEST(Vec3Test, ZeroLength)
{
    Vec3f v0;
    ASSERT_TRUE(v0.is_zero());

    v0.x = 1.0f;
    v0.y = 2.0f;
    v0.z = 3.0f;
    ASSERT_FALSE(v0.is_zero());
}

TEST(Vec3Test, Assigns)
{
    Vec3f v0;
    Vec3f v1(4.0f, 5.0f, 6.0f);
    v0 = v1;
    ASSERT_FLOAT_EQ(4.0f, v0.x);
    ASSERT_FLOAT_EQ(5.0f, v0.y);
    ASSERT_FLOAT_EQ(6.0f, v0.z);
}

TEST(Vec3Test, Adds)
{
    Vec3f v0(1.0f, 2.0f, 3.0f);
    Vec3f v1(4.0f, 5.0f, 6.0f);
    v0 += v1;
    ASSERT_FLOAT_EQ(5.0f, v0.x);
    ASSERT_FLOAT_EQ(7.0f, v0.y);
    ASSERT_FLOAT_EQ(9.0f, v0.z);
}

TEST(Vec3Test, Subtracts)
{
    Vec3f v0(1, 2, 3);
    Vec3f v1(4, 5, 6);
    v0 -= v1;
    ASSERT_FLOAT_EQ(-3.0f, v0.x);
    ASSERT_FLOAT_EQ(-3.0f, v0.y);
    ASSERT_FLOAT_EQ(-3.0f, v0.z);
}

TEST(Vec3Test, Multiplies)
{
    Vec3f v0(1.0f, 2.0f, 3.0f);
    v0 *= 2;
    ASSERT_FLOAT_EQ(2.0f, v0.x);
    ASSERT_FLOAT_EQ(4.0f, v0.y);
    ASSERT_FLOAT_EQ(6.0f, v0.z);
}
