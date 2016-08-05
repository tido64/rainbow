// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_

#include "Common/Algorithm.h"

namespace rainbow
{
    /// <summary>Structure for storing a three-dimensional vector.</summary>
    template <typename T, typename Enable = void>
    struct Vec3;

    template <typename T>
    struct Vec3<T, Arithmetic<T>>
    {
        T x, y, z;

        Vec3() : Vec3(0, 0, 0) {}
        Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

        template <typename U = T>
        auto is_zero() const -> EnableIfIntegral<U, bool>
        {
            return this->x == 0 && this->y == 0 && this->z == 0;
        }

        template <typename U = T>
        auto is_zero() const -> EnableIfFloatingPoint<U, bool>
        {
            return is_equal<T>(0.0, this->x) && is_equal<T>(0.0, this->y) &&
                   is_equal<T>(0.0, this->z);
        }

        auto operator+=(const Vec3& v) -> Vec3&
        {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }

        auto operator-=(const Vec3& v) -> Vec3&
        {
            this->x -= v.x;
            this->y -= v.y;
            this->z -= v.z;
            return *this;
        }

        auto operator*=(const T& f) -> Vec3&
        {
            this->x *= f;
            this->y *= f;
            this->z *= f;
            return *this;
        }
    };
}

using Vec3d = rainbow::Vec3<double>;
using Vec3f = rainbow::Vec3<float>;

#endif
