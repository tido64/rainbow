// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Color.h"

// TODO: The following newlines were added for Catch to create unique names.


TEST_CASE("Assigning colour to Color<char>", "[color]")
{
	Colorb cb;
	cb = 0x12345678;
	REQUIRE(cb.r == 0x12);
	REQUIRE(cb.g == 0x34);
	REQUIRE(cb.b == 0x56);
	REQUIRE(cb.a == 0x78);
}

TEST_CASE("Assigning colour to Color<float>", "[color]")
{
	Colorf cf;
	cf = 0x12345678;
	REQUIRE(Rainbow::is_equal(cf.r, 0x12 / 255.0f));
	REQUIRE(Rainbow::is_equal(cf.g, 0x34 / 255.0f));
	REQUIRE(Rainbow::is_equal(cf.b, 0x56 / 255.0f));
	REQUIRE(Rainbow::is_equal(cf.a, 0x78 / 255.0f));
}
