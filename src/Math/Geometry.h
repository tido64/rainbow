// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_GEOMETRY_H_
#define MATH_GEOMETRY_H_

#include "Common/TypeCast.h"
#include "Math/Vec2.h"

namespace rainbow
{
    /// <summary>
    ///   Applies function <paramref name="f"/> on every point on the edge of
    ///   the circle with specified center and radius.
    /// </summary>
    /// <param name="center">Center of the circle.</param>
    /// <param name="radius">Radius of the circle.</param>
    /// <param name="num_segments">Number of segments making the circle.</param>
    /// <param name="f">The function to apply.</param>
    template <typename F>
    void for_each_point_on_circle_edge(const Vec2f& center,
                                       float radius,
                                       int num_segments,
                                       F&& f)
    {
        const double increment = 2.0 * kPi<double> / num_segments;
        const float sin_inc = narrow_cast<float>(std::sin(increment));
        const float cos_inc = narrow_cast<float>(std::cos(increment));
        Vec2f r0 = Vec2f::Left;
        const Vec2f start = center + radius * r0;
        Vec2f v0 = start;
        for (int i = 1; i < num_segments; ++i)
        {
            const Vec2f r1(cos_inc * r0.x - sin_inc * r0.y,
                           sin_inc * r0.x + cos_inc * r0.y);
            const Vec2f v1 = center + radius * r1;
            f(v0, v1);
            r0 = r1;
            v0 = v1;
        }
        f(v0, start);
    }

    struct Quad
    {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        Vec2f v0, v1, v2, v3;
    };

    struct Rect
    {
        float left;    // NOLINT(misc-non-private-member-variables-in-classes)
        float bottom;  // NOLINT(misc-non-private-member-variables-in-classes)
        float right;   // NOLINT(misc-non-private-member-variables-in-classes)
        float top;     // NOLINT(misc-non-private-member-variables-in-classes)

        Rect() : Rect(0.0F, 0.0F, 0.0F, 0.0F) {}

        Rect(float left_, float bottom_, float right_, float top_)
            : left(left_), bottom(bottom_), right(right_), top(top_)
        {
        }

        [[nodiscard]] auto bottom_left() const { return Vec2f{left, bottom}; }
        [[nodiscard]] auto bottom_right() const { return Vec2f{right, bottom}; }
        [[nodiscard]] auto top_left() const { return Vec2f{left, top}; }
        [[nodiscard]] auto top_right() const { return Vec2f{right, top}; }

        friend auto operator==(const Rect& r, const Rect& s)
        {
            return r.left == s.left && r.bottom == s.bottom &&
                   r.right == s.right && r.top == s.top;
        }

        friend auto operator!=(const Rect& r, const Rect& s)
        {
            return !(r == s);
        }
    };
}  // namespace rainbow

#endif
