// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Math/Vec2.h"

#include <array>

#include <gtest/gtest.h>

using rainbow::Vec2f;
using rainbow::Vec2i;

TEST(Vec2Test, Angle)
{
    const Vec2i v0;
    const Vec2i v1(1, 1);
    ASSERT_FLOAT_EQ(0.7853982F, v0.angle(v1));
}

TEST(Vec2Test, Distance)
{
    const Vec2i v0;
    const Vec2i v1(3, 4);
    ASSERT_FLOAT_EQ(5.0F, v0.distance(v1));
}

TEST(Vec2Test, DotProduct)
{
    const Vec2i v0(3, 4);
    ASSERT_EQ((v0 * v0), 25);
}

TEST(Vec2Test, ZeroLength)
{
    Vec2i iv;
    ASSERT_TRUE(iv.is_zero());

    iv.x = 1;
    iv.y = 1;
    ASSERT_FALSE(iv.is_zero());

    Vec2f fv;
    ASSERT_TRUE(fv.is_zero());

    fv.x = 0.1F;
    fv.y = 0.1F;
    ASSERT_FALSE(fv.is_zero());
}

TEST(Vec2Test, Normalizes)
{
    constexpr double kErrorMargin = 1e-6;
    constexpr std::array<double, 20> kTestSample{
           0,    1,        0,         1,
           1,    1, 0.707107,  0.707107,
           2,    1, 0.894427,  0.447214,
           9,   -4, 0.913812, -0.406138,
        1920, 1080, 0.871576,  0.490261,
    };

    for (size_t i = 0; i < kTestSample.size(); i += 4)
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
    ASSERT_EQ(v0.x, 5);
    ASSERT_EQ(v0.y, 2);
}

TEST(Vec2Test, Translates)
{
    Vec2i v0(1, 2);
    const Vec2i v1(3, 4);

    const Vec2i v2 = v0 + v1;
    ASSERT_NE(&v0, &v2);
    ASSERT_NE(&v1, &v2);
    ASSERT_EQ(v2.x, 4);
    ASSERT_EQ(v2.y, 6);

    const Vec2i v3 = v1 + v0;
    ASSERT_NE(&v0, &v3);
    ASSERT_NE(&v1, &v3);
    ASSERT_EQ(v3.x, 4);
    ASSERT_EQ(v3.y, 6);

    v0 += v1;
    ASSERT_EQ(v0.x, 4);
    ASSERT_EQ(v0.y, 6);

    const Vec2i v4 = v0 + 1;
    ASSERT_EQ(v4.x, 5);
    ASSERT_EQ(v4.y, 7);
}

TEST(Vec2Test, TranslatesNegative)
{
    Vec2i v0(1, 2);
    const Vec2i v1(3, 4);

    const Vec2i v2 = v0 - v1;
    ASSERT_NE(&v0, &v2);
    ASSERT_NE(&v1, &v2);
    ASSERT_EQ(v2.x, -2);
    ASSERT_EQ(v2.y, -2);

    const Vec2i v3 = v1 - v0;
    ASSERT_NE(&v0, &v3);
    ASSERT_NE(&v1, &v3);
    ASSERT_EQ(v3.x, 2);
    ASSERT_EQ(v3.y, 2);

    v0 -= v1;
    ASSERT_EQ(v0.x, -2);
    ASSERT_EQ(v0.y, -2);
}

TEST(Vec2Test, ScalesUp)
{
    Vec2i v0(5, 2);

    const Vec2i v2 = v0 * 2;
    ASSERT_NE(&v0, &v2);
    ASSERT_EQ(v2.x, 10);
    ASSERT_EQ(v2.y, 4);

    const Vec2i v3 = 2 * v0;
    ASSERT_NE(&v0, &v3);
    ASSERT_EQ(v3.x, 10);
    ASSERT_EQ(v3.y, 4);

    v0 *= 2;
    ASSERT_EQ(v0.x, 10);
    ASSERT_EQ(v0.y, 4);
}

TEST(Vec2Test, ScalesDown)
{
    Vec2i v0(4, 8);

    const Vec2i v2 = v0 / 2;
    ASSERT_NE(&v0, &v2);
    ASSERT_EQ(v2.x, 2);
    ASSERT_EQ(v2.y, 4);

    v0 /= 2;
    ASSERT_EQ(v0.x, 2);
    ASSERT_EQ(v0.y, 4);
}
