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
        float angle(const Vec2<U>& v) const
        {
            return atan2f(v.y - y, v.x - x);
        }

        /// <summary>Returns the distance between two points.</summary>
        float distance(const Vec2& v) const
        {
            return sqrt(distance_sq(v));
        }

        /// <summary>Returns the distance between two points, squared.</summary>
        float distance_sq(const Vec2& v) const
        {
            const Vec2 w = *this - v;
            return w * w;
        }

        template <typename U = T>
        EnableIfIntegral<U, bool> is_zero() const
        {
            return x == 0 && y == 0;
        }

        template <typename U = T>
        EnableIfFloatingPoint<U, bool> is_zero() const
        {
            return is_equal<T>(0, x) && is_equal<T>(0, y);
        }

        Vec2 normal() const
        {
            return {-y, x};
        }

        template <typename U = T>
        EnableIfIntegral<U, Vec2<float, Arithmetic<float>>> normalize() const
        {
            const float h = sqrt(x * x + y * y);
            return {x / h, y / h};
        }

        template <typename U = T>
        EnableIfFloatingPoint<U, Vec2> normalize() const
        {
            const value_type h = sqrt(x * x + y * y);
            return {x / h, y / h};
        }

        Vec2 operator-() const
        {
            return {-x, -y};
        }

        value_type operator*(const Vec2& v) const
        {
            return x * v.x + y * v.y;
        }

        Vec2 operator*(value_type f) const
        {
            return {x * f, y * f};
        }

        Vec2 operator/(value_type f) const
        {
            return {x / f, y / f};
        }

        Vec2 operator+(const Vec2& v) const
        {
            return {x + v.x, y + v.y};
        }

        Vec2 operator+(value_type offset) const
        {
            return {x + offset, y + offset};
        }

        Vec2 operator-(const Vec2& v) const
        {
            return {x - v.x, y - v.y};
        }

        Vec2 operator-(value_type offset) const
        {
            return {x - offset, y - offset};
        }

        Vec2& operator*=(value_type f)
        {
            x *= f;
            y *= f;
            return *this;
        }

        Vec2& operator/=(value_type f)
        {
            x /= f;
            y /= f;
            return *this;
        }

        template <typename U>
        Vec2& operator+=(const Vec2<U>& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        template <typename U>
        Vec2& operator-=(const Vec2<U>& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        friend Vec2 operator*(value_type f, const Vec2& a)
        {
            return {f * a.x, f * a.y};
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
