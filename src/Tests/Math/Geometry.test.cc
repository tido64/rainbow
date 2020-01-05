// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Math/Geometry.h"

#include <gtest/gtest.h>

#include "Common/Constants.h"

using rainbow::Rect;
using rainbow::Vec2f;

TEST(GeometryTest, WalksCircleEdge)
{
    constexpr float kRadius = 10.0F;
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
            ASSERT_NEAR(v0.distance(v1), kArcLength, 0.0033F);
            ASSERT_NEAR(v1.distance(Vec2f::Zero), kRadius, 0.00002F);
        });

    ASSERT_EQ(count, kSegments);
}

TEST(GeometryTest, ReturnsRectangleVertices)
{
    const Rect rect0;

    ASSERT_EQ(rect0.bottom_left(), Vec2f::Zero);
    ASSERT_EQ(rect0.bottom_right(), Vec2f::Zero);
    ASSERT_EQ(rect0.top_left(), Vec2f::Zero);
    ASSERT_EQ(rect0.top_right(), Vec2f::Zero);

    const Rect rect1{1, 1, 0, 0};

    ASSERT_EQ(rect1.bottom_left(), Vec2f::One);
    ASSERT_EQ(rect1.bottom_right(), Vec2f::One);
    ASSERT_EQ(rect1.top_left(), Vec2f::One);
    ASSERT_EQ(rect1.top_right(), Vec2f::One);

    const Rect rect2{1, 2, 3, 4};

    ASSERT_EQ(rect2.bottom_left(), Vec2f(1, 2));
    ASSERT_EQ(rect2.bottom_right(), Vec2f(4, 2));
    ASSERT_EQ(rect2.top_left(), Vec2f(1, 6));
    ASSERT_EQ(rect2.top_right(), Vec2f(4, 6));

    ASSERT_NE(rect1, rect0);
    ASSERT_EQ(rect0, Rect{});
    ASSERT_EQ(rect1, Rect(1, 1, 0, 0));
}
