// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Vec2.h"

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
