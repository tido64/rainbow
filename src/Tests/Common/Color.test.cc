// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Color.h"

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Logging.h"

using rainbow::Color;

namespace
{
    constexpr uint8_t kAlpha = 0x78;
    constexpr uint8_t kBlue = 0x56;
    constexpr uint8_t kGreen = 0x34;
    constexpr uint8_t kRed = 0x12;

    constexpr uint32_t kTestColor = 0x12345678;
}  // namespace

TEST(ColorTest, IsWhiteByDefault)
{
    constexpr Color cb;

    ASSERT_EQ(cb.r, 255);
    ASSERT_EQ(cb.g, 255);
    ASSERT_EQ(cb.b, 255);
    ASSERT_EQ(cb.a, 255);
}

TEST(ColorTest, ConstructsColor)
{
    constexpr Color c(kTestColor);

    ASSERT_EQ(c.r, kRed);
    ASSERT_EQ(c.g, kGreen);
    ASSERT_EQ(c.b, kBlue);
    ASSERT_EQ(c.a, kAlpha);

    constexpr Color d(kRed, kGreen, kBlue, kAlpha);

    ASSERT_EQ(d.r, kRed);
    ASSERT_EQ(d.g, kGreen);
    ASSERT_EQ(d.b, kBlue);
    ASSERT_EQ(d.a, kAlpha);

    constexpr Color e(kRed, kGreen, kBlue);

    ASSERT_EQ(e.r, kRed);
    ASSERT_EQ(e.g, kGreen);
    ASSERT_EQ(e.b, kBlue);
    ASSERT_EQ(e.a, 0xff);
}

TEST(ColorTest, AssignsIntegerColor)
{
    Color c;
    c = kTestColor;

    ASSERT_EQ(c.r, kRed);
    ASSERT_EQ(c.g, kGreen);
    ASSERT_EQ(c.b, kBlue);
    ASSERT_EQ(c.a, kAlpha);
}
