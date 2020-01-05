// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_VEC2_H_
#define MATH_VEC2_H_

#include "Common/Algorithm.h"

namespace rainbow
{
    /// <summary>Structure for storing a two-dimensional vector.</summary>
    template <typename T>
    struct Vec2
    {
        using value_type = T;

        /// <summary>
        ///   Vector representing down; shorthand for <c>Vec2(0, -1)</c>.
        /// </summary>
        static const Vec2 Down;

        /// <summary>
        ///   Vector representing left; shorthand for <c>Vec2(-1, 0)</c>.
        /// </summary>
        static const Vec2 Left;

        /// <summary>
        ///   One vector; shorthand for <c>Vec2(1, 1)</c>.
        /// </summary>
        static const Vec2 One;

        /// <summary>
        ///   Vector representing right; shorthand for <c>Vec2(1, 0)</c>.
        /// </summary>
        static const Vec2 Right;

        /// <summary>
        ///   Vector representing up; shorthand for <c>Vec2(0, 1)</c>.
        /// </summary>
        static const Vec2 Up;

        /// <summary>
        ///   Zero vector; shorthand for <c>Vec2(0, 0)</c>.
        /// </summary>
        static const Vec2 Zero;

        value_type x;  // NOLINT(misc-non-private-member-variables-in-classes)
        value_type y;  // NOLINT(misc-non-private-member-variables-in-classes)

        constexpr Vec2() noexcept : Vec2(0, 0) {}
        constexpr Vec2(value_type x, value_type y) noexcept : x(x), y(y) {}

        /// <summary>
        ///   Returns the angle (in radians) between two points.
        /// </summary>
        template <typename U>
        [[nodiscard]] auto angle(const Vec2<U>& v) const
        {
            return std::atan2(v.y - y, v.x - x);
        }

        /// <summary>Returns the distance between two points.</summary>
        [[nodiscard]] auto distance(const Vec2& v) const
        {
            return std::sqrt(distance_sq(v));
        }

        /// <summary>
        ///   Returns the distance between two points, squared. This method
        ///   returns before calculating the square root as opposed to
        ///   `distance()`. This can give you some performance gains if you
        ///   don't need the exact distance.
        /// </summary>
        [[nodiscard]] auto distance_sq(const Vec2& v) const
        {
            const Vec2 w = *this - v;
            return w * w;
        }

        [[nodiscard]] auto is_zero() const -> bool
        {
            if constexpr (std::is_integral_v<T>)
                return x == 0 && y == 0;
            else
                return are_equal<T>(0, x) && are_equal<T>(0, y);
        }

        [[nodiscard]] auto normal() const { return Vec2{-y, x}; }

        [[nodiscard]] auto normalize() const
        {
            if constexpr (std::is_integral_v<T>)
            {
                const float h = std::sqrt(x * x + y * y);
                return Vec2<float>{x / h, y / h};
            }
            else
            {
                const value_type h = std::sqrt(x * x + y * y);
                return Vec2{x / h, y / h};
            }
        }

        auto operator-() const { return Vec2{-x, -y}; }

        auto operator*(const Vec2& v) const { return x * v.x + y * v.y; }

        auto operator*(value_type f) const { return Vec2{x * f, y * f}; }

        auto operator/(value_type f) const { return Vec2{x / f, y / f}; }

        auto operator+(const Vec2& v) const { return Vec2{x + v.x, y + v.y}; }

        auto operator+(value_type offset) const
        {
            return Vec2{x + offset, y + offset};
        }

        auto operator-(const Vec2& v) const { return Vec2{x - v.x, y - v.y}; }

        auto operator-(value_type offset) const
        {
            return Vec2{x - offset, y - offset};
        }

        auto operator*=(value_type f) -> Vec2&
        {
            x *= f;
            y *= f;
            return *this;
        }

        auto operator/=(value_type f) -> Vec2&
        {
            x /= f;
            y /= f;
            return *this;
        }

        template <typename U>
        auto operator+=(const Vec2<U>& v) -> Vec2&
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        template <typename U>
        auto operator-=(const Vec2<U>& v) -> Vec2&
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        friend auto operator*(value_type f, const Vec2& a)
        {
            return Vec2{f * a.x, f * a.y};
        }

        friend auto operator==(const Vec2& a, const Vec2& b) -> bool
        {
            return a.x == b.x && a.y == b.y;
        }
    };

    template <typename T>
    const Vec2<T> Vec2<T>::Down(0, -1);

    template <typename T>
    const Vec2<T> Vec2<T>::Left(-1, 0);

    template <typename T>
    const Vec2<T> Vec2<T>::One(1, 1);

    template <typename T>
    const Vec2<T> Vec2<T>::Right(1, 0);

    template <typename T>
    const Vec2<T> Vec2<T>::Up(0, 1);

    template <typename T>
    const Vec2<T> Vec2<T>::Zero;

    using Vec2f = Vec2<float>;
    using Vec2i = Vec2<int32_t>;
    using Vec2u = Vec2<uint32_t>;
}  // namespace rainbow

#endif
