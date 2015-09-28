// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

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

TEST(ColorTest, IsWhiteByDefault)
{
	const Colorb cb;
	ASSERT_EQ(255, cb.r);
	ASSERT_EQ(255, cb.g);
	ASSERT_EQ(255, cb.b);
	ASSERT_EQ(255, cb.a);

	const Colorf cf;
	ASSERT_EQ(1.0f, cf.r);
	ASSERT_EQ(1.0f, cf.g);
	ASSERT_EQ(1.0f, cf.b);
	ASSERT_EQ(1.0f, cf.a);
}

TEST(ColorTest, ConstructsColorWithTypeChar)
{
	const Colorb c(kTestColor);
	ASSERT_EQ(kRed, c.r);
	ASSERT_EQ(kGreen, c.g);
	ASSERT_EQ(kBlue, c.b);
	ASSERT_EQ(kAlpha, c.a);

	const Colorb d(kRed, kGreen, kBlue, kAlpha);
	ASSERT_EQ(kRed, d.r);
	ASSERT_EQ(kGreen, d.g);
	ASSERT_EQ(kBlue, d.b);
	ASSERT_EQ(kAlpha, d.a);

	const Colorb e(kRed, kGreen, kBlue);
	ASSERT_EQ(kRed, e.r);
	ASSERT_EQ(kGreen, e.g);
	ASSERT_EQ(kBlue, e.b);
	ASSERT_EQ(0xff, e.a);
}

TEST(ColorTest, AssignsIntegerColorWithTypeChar)
{
	Colorb c;
	c = kTestColor;
	ASSERT_EQ(kRed, c.r);
	ASSERT_EQ(kGreen, c.g);
	ASSERT_EQ(kBlue, c.b);
	ASSERT_EQ(kAlpha, c.a);
}

TEST(ColorTest, ConstructsColorWithTypeFloat)
{
	const Colorf c(kTestColor);
	ASSERT_FLOAT_EQ(kRedF, c.r);
	ASSERT_FLOAT_EQ(kGreenF, c.g);
	ASSERT_FLOAT_EQ(kBlueF, c.b);
	ASSERT_FLOAT_EQ(kAlphaF, c.a);

	const Colorf d(kRedF, kGreenF, kBlueF, kAlphaF);
	ASSERT_FLOAT_EQ(kRedF, d.r);
	ASSERT_FLOAT_EQ(kGreenF, d.g);
	ASSERT_FLOAT_EQ(kBlueF, d.b);
	ASSERT_FLOAT_EQ(kAlphaF, d.a);

	const Colorf e(kRedF, kGreenF, kBlueF);
	ASSERT_FLOAT_EQ(kRedF, e.r);
	ASSERT_FLOAT_EQ(kGreenF, e.g);
	ASSERT_FLOAT_EQ(kBlueF, e.b);
	ASSERT_FLOAT_EQ(1.0f, e.a);
}

TEST(ColorTest, AssignsIntegerColorWithTypeFloat)
{
	Colorf c;
	c = kTestColor;
	ASSERT_FLOAT_EQ(kRedF, c.r);
	ASSERT_FLOAT_EQ(kGreenF, c.g);
	ASSERT_FLOAT_EQ(kBlueF, c.b);
	ASSERT_FLOAT_EQ(kAlphaF, c.a);
}
