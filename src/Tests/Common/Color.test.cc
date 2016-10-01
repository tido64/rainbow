// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Color.h"
#include "Common/Logging.h"

namespace
{
    constexpr uint8_t kAlpha = 0x78;
    constexpr uint8_t kBlue = 0x56;
    constexpr uint8_t kGreen = 0x34;
    constexpr uint8_t kRed = 0x12;

    constexpr float kAlphaF = kAlpha / 255.0f;
    constexpr float kBlueF = kBlue / 255.0f;
    constexpr float kGreenF = kGreen / 255.0f;
    constexpr float kRedF = kRed / 255.0f;

    constexpr uint32_t kTestColor = 0x12345678;
}

TEST(ColorTest, IsWhiteByDefault)
{
    constexpr Colorb cb;

    ASSERT_EQ(255, cb.r);
    ASSERT_EQ(255, cb.g);
    ASSERT_EQ(255, cb.b);
    ASSERT_EQ(255, cb.a);

    constexpr Colorf cf;

    ASSERT_EQ(1.0f, cf.r);
    ASSERT_EQ(1.0f, cf.g);
    ASSERT_EQ(1.0f, cf.b);
    ASSERT_EQ(1.0f, cf.a);
}

TEST(ColorTest, ConstructsColorWithBytes)
{
    constexpr Colorb c(kTestColor);

    ASSERT_EQ(kRed, c.r);
    ASSERT_EQ(kGreen, c.g);
    ASSERT_EQ(kBlue, c.b);
    ASSERT_EQ(kAlpha, c.a);

    constexpr Colorb d(kRed, kGreen, kBlue, kAlpha);

    ASSERT_EQ(kRed, d.r);
    ASSERT_EQ(kGreen, d.g);
    ASSERT_EQ(kBlue, d.b);
    ASSERT_EQ(kAlpha, d.a);

    constexpr Colorb e(kRed, kGreen, kBlue);

    ASSERT_EQ(kRed, e.r);
    ASSERT_EQ(kGreen, e.g);
    ASSERT_EQ(kBlue, e.b);
    ASSERT_EQ(0xff, e.a);
}

TEST(ColorTest, AssignsIntegerColorWithBytes)
{
    Colorb c;
    c = kTestColor;

    ASSERT_EQ(kRed, c.r);
    ASSERT_EQ(kGreen, c.g);
    ASSERT_EQ(kBlue, c.b);
    ASSERT_EQ(kAlpha, c.a);
}

TEST(ColorTest, ConstructsColorWithFloats)
{
    constexpr Colorf c(kTestColor);

    ASSERT_FLOAT_EQ(kRedF, c.r);
    ASSERT_FLOAT_EQ(kGreenF, c.g);
    ASSERT_FLOAT_EQ(kBlueF, c.b);
    ASSERT_FLOAT_EQ(kAlphaF, c.a);

    constexpr Colorf d(kRedF, kGreenF, kBlueF, kAlphaF);

    ASSERT_FLOAT_EQ(kRedF, d.r);
    ASSERT_FLOAT_EQ(kGreenF, d.g);
    ASSERT_FLOAT_EQ(kBlueF, d.b);
    ASSERT_FLOAT_EQ(kAlphaF, d.a);

    constexpr Colorf e(kRedF, kGreenF, kBlueF);

    ASSERT_FLOAT_EQ(kRedF, e.r);
    ASSERT_FLOAT_EQ(kGreenF, e.g);
    ASSERT_FLOAT_EQ(kBlueF, e.b);
    ASSERT_FLOAT_EQ(1.0f, e.a);
}

TEST(ColorTest, AssignsIntegerColorWithFloats)
{
    Colorf c;
    c = kTestColor;

    ASSERT_FLOAT_EQ(kRedF, c.r);
    ASSERT_FLOAT_EQ(kGreenF, c.g);
    ASSERT_FLOAT_EQ(kBlueF, c.b);
    ASSERT_FLOAT_EQ(kAlphaF, c.a);
}
