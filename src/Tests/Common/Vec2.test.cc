// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/Vec2.h"

TEST_CASE("Angle between two Vec2s", "[vec2]")
{
	const Vec2i v0;
	const Vec2i v1(1, 1);
	REQUIRE(rainbow::is_equal(v0.angle(v1), 0.7853982f));
}

TEST_CASE("Distance between two Vec2s", "[vec2]")
{
	const Vec2i v0;
	const Vec2i v1(3, 4);
	REQUIRE(rainbow::is_equal(v0.distance(v1), 5.0f));
}

TEST_CASE("Dot product of two Vec2s", "[vec2]")
{
	const Vec2i v0(3, 4);
	REQUIRE((v0 * v0) == 25);
}

TEST_CASE("Vec2s can be checked for zero length", "[vec2]")
{
	Vec2i v0;
	REQUIRE(v0.is_zero());

	v0.x = 1;
	v0.y = 1;
	REQUIRE_FALSE(v0.is_zero());
}

TEST_CASE("Vec2s can be assigned", "[vec2]")
{
	Vec2i v0;
	const Vec2i v1(5, 2);
	v0 = v1;
	REQUIRE(v0.x == 5);
	REQUIRE(v0.y == 2);
}

TEST_CASE("Vec2s can be translated (addition)", "[vec2]")
{
	Vec2i v0(1, 2);
	const Vec2i v1(3, 4);

	const Vec2i v2 = v0 + v1;
	REQUIRE(&v0 != &v2);
	REQUIRE(&v1 != &v2);
	REQUIRE(v2.x == 4);
	REQUIRE(v2.y == 6);

	const Vec2i v3 = v1 + v0;
	REQUIRE(&v0 != &v3);
	REQUIRE(&v1 != &v3);
	REQUIRE(v3.x == 4);
	REQUIRE(v3.y == 6);

	v0 += v1;
	REQUIRE(v0.x == 4);
	REQUIRE(v0.y == 6);
}

TEST_CASE("Vec2s can be translated (subtraction)", "[vec2]")
{
	Vec2i v0(1, 2);
	const Vec2i v1(3, 4);

	const Vec2i v2 = v0 - v1;
	REQUIRE(&v0 != &v2);
	REQUIRE(&v1 != &v2);
	REQUIRE(v2.x == -2);
	REQUIRE(v2.y == -2);

	const Vec2i v3 = v1 - v0;
	REQUIRE(&v0 != &v3);
	REQUIRE(&v1 != &v3);
	REQUIRE(v3.x == 2);
	REQUIRE(v3.y == 2);

	v0 -= v1;
	REQUIRE(v0.x == -2);
	REQUIRE(v0.y == -2);
}

TEST_CASE("Vec2s can be scaled up", "[vec2]")
{
	Vec2i v0(5, 2);
	const Vec2i v1(3, 4);

	const Vec2i v2 = 2 * v0;
	REQUIRE(&v0 != &v2);
	REQUIRE(v2.x == 10);
	REQUIRE(v2.y == 4);

	v0 *= 2;
	REQUIRE(v0.x == 10);
	REQUIRE(v0.y == 4);
}

TEST_CASE("Vec2s can be scaled down", "[vec2]")
{
	Vec2i v0(4, 8);

	const Vec2i v2 = v0 / 2;
	REQUIRE(&v0 != &v2);
	REQUIRE(v2.x == 2);
	REQUIRE(v2.y == 4);

	v0 /= 2;
	REQUIRE(v0.x == 2);
	REQUIRE(v0.y == 4);
}
