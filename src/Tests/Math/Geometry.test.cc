// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Constants.h"
#include "Math/Geometry.h"

using rainbow::Rect;
using rainbow::Vec2f;

TEST(GeometryTest, WalksCircleEdge)
{
    constexpr float kRadius = 10.0f;
    constexpr float kCircumference = 2 * kRadius * rainbow::kPi<float>;
    constexpr int kSegments = 32;
    constexpr float kArcLength = kCircumference / kSegments;

    int count = 0;
    rainbow::for_each_point_on_circle_edge(
        Vec2f::Zero,
        kRadius,
        kSegments,
        [=, &count](const Vec2f& v0, const Vec2f& v1) mutable {
            ++count;
            ASSERT_NEAR(v0.distance(v1), kArcLength, 0.0033f);
            ASSERT_NEAR(v1.distance(Vec2f::Zero), kRadius, 0.00002f);
        });

    ASSERT_EQ(kSegments, count);
}

TEST(GeometryTest, ReturnsRectangleVertices)
{
    const Rect rect0;

    ASSERT_EQ(Vec2f::Zero, rect0.bottom_left());
    ASSERT_EQ(Vec2f::Zero, rect0.bottom_right());
    ASSERT_EQ(Vec2f::Zero, rect0.top_left());
    ASSERT_EQ(Vec2f::Zero, rect0.top_right());

    const Rect rect1{1, 1, 1, 1};

    ASSERT_EQ(Vec2f::One, rect1.bottom_left());
    ASSERT_EQ(Vec2f::One, rect1.bottom_right());
    ASSERT_EQ(Vec2f::One, rect1.top_left());
    ASSERT_EQ(Vec2f::One, rect1.top_right());

    const Rect rect2{1, 2, 3, 4};

    ASSERT_EQ(Vec2f(1, 2), rect2.bottom_left());
    ASSERT_EQ(Vec2f(3, 2), rect2.bottom_right());
    ASSERT_EQ(Vec2f(1, 4), rect2.top_left());
    ASSERT_EQ(Vec2f(3, 4), rect2.top_right());

    ASSERT_NE(rect0, rect1);
    ASSERT_EQ(Rect(), rect0);
    ASSERT_EQ(Rect(1, 1, 1, 1), rect1);
}
