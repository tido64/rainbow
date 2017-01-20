// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Math/Vec2.h"

using rainbow::Vec2f;
using rainbow::Vec2i;

TEST(Vec2Test, Angle)
{
    const Vec2i v0;
    const Vec2i v1(1, 1);
    ASSERT_FLOAT_EQ(0.7853982f, v0.angle(v1));
}

TEST(Vec2Test, Distance)
{
    const Vec2i v0;
    const Vec2i v1(3, 4);
    ASSERT_FLOAT_EQ(5.0f, v0.distance(v1));
}

TEST(Vec2Test, DotProduct)
{
    const Vec2i v0(3, 4);
    ASSERT_EQ(25, (v0 * v0));
}

TEST(Vec2Test, ZeroLength)
{
    Vec2i v0;
    ASSERT_TRUE(v0.is_zero());

    v0.x = 1;
    v0.y = 1;
    ASSERT_FALSE(v0.is_zero());
}

TEST(Vec2Test, Normalizes)
{
    constexpr double kErrorMargin = 1e-6;
    constexpr double kTestSample[]{
           0,    1,        0,         1,
           1,    1, 0.707107,  0.707107,
           2,    1, 0.894427,  0.447214,
           9,   -4, 0.913812, -0.406138,
        1920, 1080, 0.871576,  0.490261};

    for (size_t i = 0; i < rainbow::array_size(kTestSample); i += 4)
    {
        const auto& v = Vec2f(kTestSample[i], kTestSample[i + 1]).normalize();
        ASSERT_NEAR(kTestSample[i + 2], v.x, kErrorMargin);
        ASSERT_NEAR(kTestSample[i + 3], v.y, kErrorMargin);
    }
}

TEST(Vec2Test, Assigns)
{
    Vec2i v0;
    const Vec2i v1(5, 2);
    v0 = v1;
    ASSERT_EQ(5, v0.x);
    ASSERT_EQ(2, v0.y);
}

TEST(Vec2Test, Translates)
{
    Vec2i v0(1, 2);
    const Vec2i v1(3, 4);

    const Vec2i v2 = v0 + v1;
    ASSERT_NE(&v2, &v0);
    ASSERT_NE(&v2, &v1);
    ASSERT_EQ(4, v2.x);
    ASSERT_EQ(6, v2.y);

    const Vec2i v3 = v1 + v0;
    ASSERT_NE(&v3, &v0);
    ASSERT_NE(&v3, &v1);
    ASSERT_EQ(4, v3.x);
    ASSERT_EQ(6, v3.y);

    v0 += v1;
    ASSERT_EQ(4, v0.x);
    ASSERT_EQ(6, v0.y);

    const Vec2i v4 = v0 + 1;
    ASSERT_EQ(5, v4.x);
    ASSERT_EQ(7, v4.y);
}

TEST(Vec2Test, TranslatesNegative)
{
    Vec2i v0(1, 2);
    const Vec2i v1(3, 4);

    const Vec2i v2 = v0 - v1;
    ASSERT_NE(&v2, &v0);
    ASSERT_NE(&v2, &v1);
    ASSERT_EQ(-2, v2.x);
    ASSERT_EQ(-2, v2.y);

    const Vec2i v3 = v1 - v0;
    ASSERT_NE(&v3, &v0);
    ASSERT_NE(&v3, &v1);
    ASSERT_EQ(2, v3.x);
    ASSERT_EQ(2, v3.y);

    v0 -= v1;
    ASSERT_EQ(-2, v0.x);
    ASSERT_EQ(-2, v0.y);
}

TEST(Vec2Test, ScalesUp)
{
    Vec2i v0(5, 2);
    const Vec2i v1(3, 4);

    const Vec2i v2 = 2 * v0;
    ASSERT_NE(&v2, &v0);
    ASSERT_EQ(10, v2.x);
    ASSERT_EQ(4, v2.y);

    v0 *= 2;
    ASSERT_EQ(10, v0.x);
    ASSERT_EQ(4, v0.y);
}

TEST(Vec2Test, ScalesDown)
{
    Vec2i v0(4, 8);

    const Vec2i v2 = v0 / 2;
    ASSERT_NE(&v2, &v0);
    ASSERT_EQ(2, v2.x);
    ASSERT_EQ(4, v2.y);

    v0 /= 2;
    ASSERT_EQ(2, v0.x);
    ASSERT_EQ(4, v0.y);
}
