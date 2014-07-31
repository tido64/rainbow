// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VEC3_H_
#define COMMON_VEC3_H_

#include "Common/Algorithm.h"

namespace Rainbow
{
	/// Structure for storing a three-dimensional vector.
	template<typename T, typename Enable = void>
	struct Vec3;

	template<typename T>
	struct Vec3<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
	{
		T x, y, z;

		Vec3() : x(0), y(0), z(0) { }
		Vec3(const T x, const T y, const T z) : x(x), y(y), z(z) { }

		template<typename U = T>
		typename std::enable_if<std::is_integral<U>::value, bool>::type
		is_zero() const
		{
			return this->x == 0 && this->y == 0 && this->z == 0;
		}

		template<typename U = T>
		typename std::enable_if<std::is_floating_point<U>::value, bool>::type
		is_zero() const
		{
			return is_equal<T>(0.0, this->x) && is_equal<T>(0.0, this->y) &&
			    is_equal<T>(0.0, this->z);
		}

		Vec3& operator+=(const Vec3 &v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}

		Vec3& operator-=(const Vec3 &v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}

		Vec3& operator*=(const T &f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			return *this;
		}
	};
}

using Vec3d = Rainbow::Vec3<double>;
using Vec3f = Rainbow::Vec3<float>;

#endif
