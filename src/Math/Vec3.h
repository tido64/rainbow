// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_

#include "Common/Algorithm.h"

namespace rainbow
{
    /// <summary>Structure for storing a three-dimensional vector.</summary>
    template <typename T>
    struct Vec3 {
        T x, y, z;  // NOLINT(misc-non-private-member-variables-in-classes)

        Vec3() : Vec3(0, 0, 0) {}
        Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

        [[nodiscard]] auto is_zero() const -> bool
        {
            if constexpr (std::is_integral_v<T>) {
                return x == 0 && y == 0 && z == 0;
            } else {
                return are_equal<T>(0.0, x) && are_equal<T>(0.0, y) &&
                       are_equal<T>(0.0, z);
            }
        }

        auto operator+=(const Vec3& v) -> Vec3&
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        auto operator-=(const Vec3& v) -> Vec3&
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        auto operator*=(const T& f) -> Vec3&
        {
            x *= f;
            y *= f;
            z *= f;
            return *this;
        }
    };

    using Vec3d = Vec3<double>;
    using Vec3f = Vec3<float>;
}  // namespace rainbow

#endif
