// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Color.h"
#include "Common/Logging.h"

using rainbow::Color;

namespace
{
    constexpr uint8_t kAlpha = 0x78;
    constexpr uint8_t kBlue = 0x56;
    constexpr uint8_t kGreen = 0x34;
    constexpr uint8_t kRed = 0x12;

    constexpr uint32_t kTestColor = 0x12345678;
}

TEST(ColorTest, IsWhiteByDefault)
{
    constexpr Color cb;

    ASSERT_EQ(255, cb.r);
    ASSERT_EQ(255, cb.g);
    ASSERT_EQ(255, cb.b);
    ASSERT_EQ(255, cb.a);
}

TEST(ColorTest, ConstructsColor)
{
    constexpr Color c(kTestColor);

    ASSERT_EQ(kRed, c.r);
    ASSERT_EQ(kGreen, c.g);
    ASSERT_EQ(kBlue, c.b);
    ASSERT_EQ(kAlpha, c.a);

    constexpr Color d(kRed, kGreen, kBlue, kAlpha);

    ASSERT_EQ(kRed, d.r);
    ASSERT_EQ(kGreen, d.g);
    ASSERT_EQ(kBlue, d.b);
    ASSERT_EQ(kAlpha, d.a);

    constexpr Color e(kRed, kGreen, kBlue);

    ASSERT_EQ(kRed, e.r);
    ASSERT_EQ(kGreen, e.g);
    ASSERT_EQ(kBlue, e.b);
    ASSERT_EQ(0xff, e.a);
}

TEST(ColorTest, AssignsIntegerColor)
{
    Color c;
    c = kTestColor;

    ASSERT_EQ(kRed, c.r);
    ASSERT_EQ(kGreen, c.g);
    ASSERT_EQ(kBlue, c.b);
    ASSERT_EQ(kAlpha, c.a);
}
