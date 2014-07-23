// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Vec2.h"

// TODO: The following newlines were added for Catch to create unique names.




TEST_CASE("Angle between two Vec2s", "[vec2]")
{
	Vec2i v0;
	Vec2i v1(1, 1);
	REQUIRE(Rainbow::is_equal(v0.angle(v1), 0.7853982f));
}

TEST_CASE("Distance between two Vec2s", "[vec2]")
{
	Vec2i v0;
	Vec2i v1(3, 4);
	REQUIRE(Rainbow::is_equal(v0.distance(v1), 5.0f));
}

TEST_CASE("Dot product of two Vec2s", "[vec2]")
{
	Vec2i v0(3, 4);
	REQUIRE(v0.dot(v0) == 25);
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
	Vec2i v1(5, 2);
	v0 = v1;
	REQUIRE(v0.x == 5);
	REQUIRE(v0.y == 2);
}

TEST_CASE("Vec2s can be added to one another", "[vec2]")
{
	Vec2i v0(1, 2);
	Vec2i v1(3, 4);
	v0 += v1;
	REQUIRE(v0.x == 4);
	REQUIRE(v0.y == 6);
}

// TODO: The following newlines were added for Catch to create unique names.

TEST_CASE("Vec2s can be subtracted from one another", "[vec2]")
{
	Vec2i v0(1, 2);
	Vec2i v1(3, 4);
	v0 -= v1;
	REQUIRE(v0.x == -2);
	REQUIRE(v0.y == -2);
}

TEST_CASE("Vec2s can be multiplied with each other", "[vec2]")
{
	Vec2i v0(5, 2);
	v0 *= 2;
	REQUIRE(v0.x == 10);
	REQUIRE(v0.y == 4);
}
