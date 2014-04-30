// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Vec3.h"

// TODO: The following newlines were added for Catch to create unique names.





TEST_CASE("Vec3s can be checked for zero length", "[vec3]")
{
	Vec3f v0;
	REQUIRE(v0.is_zero());

	v0.x = 1.0f;
	v0.y = 2.0f;
	v0.z = 3.0f;
	REQUIRE_FALSE(v0.is_zero());
}

TEST_CASE("Vec3s can be assigned", "[vec3]")
{
	Vec3f v0;
	Vec3f v1(4.0f, 5.0f, 6.0f);
	v0 = v1;
	REQUIRE(Rainbow::isequal(v0.x, 4.0f));
	REQUIRE(Rainbow::isequal(v0.y, 5.0f));
	REQUIRE(Rainbow::isequal(v0.z, 6.0f));
}

TEST_CASE("Vec3s can be added to one another", "[vec3]")
{
	Vec3f v0(1.0f, 2.0f, 3.0f);
	Vec3f v1(4.0f, 5.0f, 6.0f);
	v0 += v1;
	REQUIRE(Rainbow::isequal(v0.x, 5.0f));
	REQUIRE(Rainbow::isequal(v0.y, 7.0f));
	REQUIRE(Rainbow::isequal(v0.z, 9.0f));
}

TEST_CASE("Vec3s can be subtracted from one another", "[vec3]")
{
	Vec3f v0(1, 2, 3);
	Vec3f v1(4, 5, 6);
	v0 -= v1;
	REQUIRE(Rainbow::isequal(v0.x, -3.0f));
	REQUIRE(Rainbow::isequal(v0.y, -3.0f));
	REQUIRE(Rainbow::isequal(v0.z, -3.0f));
}

// TODO: The following newlines were added for Catch to create unique names.

TEST_CASE("Vec3s can be multiplied with each other", "[vec3]")
{
	Vec3f v0(1.0f, 2.0f, 3.0f);
	v0 *= 2;
	REQUIRE(Rainbow::isequal(v0.x, 2.0f));
	REQUIRE(Rainbow::isequal(v0.y, 4.0f));
	REQUIRE(Rainbow::isequal(v0.z, 6.0f));
}
