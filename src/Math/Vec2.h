// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_VEC2_H_
#define MATH_VEC2_H_

#include "Common/Algorithm.h"

namespace rainbow
{
    /// <summary>Structure for storing a two-dimensional vector.</summary>
    template <typename T, typename Enable = void>
    struct Vec2;

    template <typename T>
    struct Vec2<T, Arithmetic<T>>
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

        value_type x;
        value_type y;

        constexpr Vec2() : Vec2(0, 0) {}
        constexpr Vec2(value_type x, value_type y) : x(x), y(y) {}

        /// <summary>
        ///   Returns the angle (in radians) between two points.
        /// </summary>
        template <typename U>
        auto angle(const Vec2<U>& v) const -> float
        {
            return atan2f(v.y - y, v.x - x);
        }

        /// <summary>Returns the distance between two points.</summary>
        auto distance(const Vec2& v) const -> float
        {
            return sqrt(distance_sq(v));
        }

        /// <summary>Returns the distance between two points, squared.</summary>
        auto distance_sq(const Vec2& v) const -> float
        {
            const Vec2 w = *this - v;
            return w * w;
        }

        template <typename U = T>
        auto is_zero() const -> EnableIfIntegral<U, bool>
        {
            return x == 0 && y == 0;
        }

        template <typename U = T>
        auto is_zero() const -> EnableIfFloatingPoint<U, bool>
        {
            return are_equal<T>(0, x) && are_equal<T>(0, y);
        }

        auto normal() const
        {
            return Vec2{-y, x};
        }

        template <typename U = T>
        auto normalize() const
            -> EnableIfIntegral<U, Vec2<float, Arithmetic<float>>>
        {
            const float h = sqrt(x * x + y * y);
            return {x / h, y / h};
        }

        template <typename U = T>
        auto normalize() const -> EnableIfFloatingPoint<U, Vec2>
        {
            const value_type h = sqrt(x * x + y * y);
            return {x / h, y / h};
        }

        auto operator-() const
        {
            return Vec2{-x, -y};
        }

        auto operator*(const Vec2& v) const
        {
            return x * v.x + y * v.y;
        }

        auto operator*(value_type f) const
        {
            return Vec2{x * f, y * f};
        }

        auto operator/(value_type f) const
        {
            return Vec2{x / f, y / f};
        }

        auto operator+(const Vec2& v) const
        {
            return Vec2{x + v.x, y + v.y};
        }

        auto operator+(value_type offset) const
        {
            return Vec2{x + offset, y + offset};
        }

        auto operator-(const Vec2& v) const
        {
            return Vec2{x - v.x, y - v.y};
        }

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

        friend bool operator==(const Vec2& a, const Vec2& b)
        {
            return a.x == b.x && a.y == b.y;
        }
    };

    template <typename T>
    const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Down(0, -1);

    template <typename T>
    const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Left(-1, 0);

    template <typename T>
    const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::One(1, 1);

    template <typename T>
    const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Right(1, 0);

    template <typename T>
    const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Up(0, 1);

    template <typename T>
    const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Zero;
}

using Vec2f = rainbow::Vec2<float>;
using Vec2i = rainbow::Vec2<int>;
using Vec2u = rainbow::Vec2<unsigned>;

#endif
