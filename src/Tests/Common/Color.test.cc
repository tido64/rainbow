// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/Algorithm.h"
#include "Common/Color.h"
#include "Common/Logging.h"

namespace
{
	const char kAlpha = 0x78;
	const char kBlue = 0x56;
	const char kGreen = 0x34;
	const char kRed = 0x12;

	const float kAlphaF = kAlpha / 255.0f;
	const float kBlueF = kBlue / 255.0f;
	const float kGreenF = kGreen / 255.0f;
	const float kRedF = kRed / 255.0f;

	const unsigned int kTestColor = 0x12345678;
}

TEST_CASE("Color<T> is white by default", "[color]")
{
	const Colorb cb;
	REQUIRE(cb.r == 255);
	REQUIRE(cb.g == 255);
	REQUIRE(cb.b == 255);
	REQUIRE(cb.a == 255);

	const Colorf cf;
	REQUIRE(cf.r == 1.0f);
	REQUIRE(cf.g == 1.0f);
	REQUIRE(cf.b == 1.0f);
	REQUIRE(cf.a == 1.0f);
}

TEST_CASE("Can construct a Color<char> with an integer", "[color]")
{
	const Colorb c(kTestColor);
	REQUIRE(c.r == kRed);
	REQUIRE(c.g == kGreen);
	REQUIRE(c.b == kBlue);
	REQUIRE(c.a == kAlpha);
}

TEST_CASE("Can construct a Color<char> component-wise", "[color]")
{
	const Colorb c(kRed, kGreen, kBlue, kAlpha);
	REQUIRE(c.r == kRed);
	REQUIRE(c.g == kGreen);
	REQUIRE(c.b == kBlue);
	REQUIRE(c.a == kAlpha);

	const Colorb d(kRed, kGreen, kBlue);
	REQUIRE(d.r == kRed);
	REQUIRE(d.g == kGreen);
	REQUIRE(d.b == kBlue);
	REQUIRE(d.a == 0xff);
}

TEST_CASE("Can assign an integer to Color<char>", "[color]")
{
	Colorb c;
	c = kTestColor;
	REQUIRE(c.r == kRed);
	REQUIRE(c.g == kGreen);
	REQUIRE(c.b == kBlue);
	REQUIRE(c.a == kAlpha);
}

TEST_CASE("Can construct a Color<float> with an integer", "[color]")
{
	const Colorf c(kTestColor);
	REQUIRE(c.r == Approx(kRedF));
	REQUIRE(c.g == Approx(kGreenF));
	REQUIRE(c.b == Approx(kBlueF));
	REQUIRE(c.a == Approx(kAlphaF));
}

TEST_CASE("Can construct a Color<float> component-wise", "[color]")
{
	const Colorf c(kRedF, kGreenF, kBlueF, kAlphaF);
	REQUIRE(c.r == Approx(kRedF));
	REQUIRE(c.g == Approx(kGreenF));
	REQUIRE(c.b == Approx(kBlueF));
	REQUIRE(c.a == Approx(kAlphaF));

	const Colorf d(kRedF, kGreenF, kBlueF);
	REQUIRE(d.r == Approx(kRedF));
	REQUIRE(d.g == Approx(kGreenF));
	REQUIRE(d.b == Approx(kBlueF));
	REQUIRE(d.a == Approx(1.0f));
}

TEST_CASE("Can assign an integer to Color<float>", "[color]")
{
	Colorf c;
	c = kTestColor;
	REQUIRE(c.r == Approx(kRedF));
	REQUIRE(c.g == Approx(kGreenF));
	REQUIRE(c.b == Approx(kBlueF));
	REQUIRE(c.a == Approx(kAlphaF));
}
