// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VEC2_H_
#define COMMON_VEC2_H_

#include "Common/Algorithm.h"

namespace rainbow
{
	/// Structure for storing a two-dimensional vector.
	template<typename T, typename Enable = void>
	struct Vec2;

	template<typename T>
	struct Vec2<T, Arithmetic<T>>
	{
		using value_type = T;

		T x;
		T y;

		Vec2() : Vec2(0, 0) {}
		Vec2(const T x, const T y) : x(x), y(y) {}

		/// Returns the angle (in radians) between two points.
		float angle(const Vec2 &v) const
		{
			return atan2f(v.y - this->y, v.x - this->x);
		}

		/// Returns the distance between two points.
		float distance(const Vec2 &v) const
		{
			return sqrt(distance_sq(v));
		}

		/// Returns the distance between two points, squared.
		float distance_sq(const Vec2 &v) const
		{
			const Vec2 w = *this - v;
			return w * w;
		}

		template<typename U = T>
		EnableIfIntegral<U, bool> is_zero() const
		{
			return this->x == 0 && this->y == 0;
		}

		template<typename U = T>
		EnableIfFloatingPoint<U, bool> is_zero() const
		{
			return is_equal<T>(0.0, this->x) && is_equal<T>(0.0, this->y);
		}

		Vec2& operator+=(const Vec2 &v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}

		Vec2& operator-=(const Vec2 &v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}

		Vec2& operator*=(const T &f)
		{
			this->x *= f;
			this->y *= f;
			return *this;
		}

		Vec2& operator/=(const T &f)
		{
			this->x /= f;
			this->y /= f;
			return *this;
		}

		friend Vec2 operator+(const Vec2 &a, const T offset)
		{
			return Vec2(a.x + offset, a.y + offset);
		}

		friend Vec2 operator+(const Vec2 &a, const Vec2 &b)
		{
			return Vec2(a.x + b.x, a.y + b.y);
		}

		friend Vec2 operator-(const Vec2 &a, const T offset)
		{
			return Vec2(a.x - offset, a.y - offset);
		}

		friend Vec2 operator-(const Vec2 &a, const Vec2 &b)
		{
			return Vec2(a.x - b.x, a.y - b.y);
		}

		friend Vec2 operator*(const Vec2 &a, const T f)
		{
			return Vec2(a.x * f, a.y * f);
		}

		friend T operator*(const Vec2 &a, const Vec2 &b)
		{
			return a.x * b.x + a.y * b.y;
		}

		friend Vec2 operator/(const Vec2 &a, const T f)
		{
			return Vec2(a.x / f, a.y / f);
		}

		friend bool operator==(const Vec2 &a, const Vec2 &b)
		{
			return a.x == b.x && a.y == b.y;
		}
	};
}

using Vec2f = rainbow::Vec2<float>;
using Vec2i = rainbow::Vec2<int>;
using Vec2u = rainbow::Vec2<unsigned>;

#endif
