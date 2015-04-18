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

		/// Vector representing down; shorthand for Vec2(0, -1).
		static const Vec2 Down;

		/// Vector representing left; shorthand for Vec2(-1, 0).
		static const Vec2 Left;

		/// One vector; shorthand for Vec2(1, 1).
		static const Vec2 One;

		/// Vector representing right; shorthand for Vec2(1, 0).
		static const Vec2 Right;

		/// Vector representing up; shorthand for Vec2(0, 1).
		static const Vec2 Up;

		/// Zero vector; shorthand for Vec2(0, 0).
		static const Vec2 Zero;

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
			return is_equal<T>(0, this->x) && is_equal<T>(0, this->y);
		}

		Vec2 operator-() const { return Vec2( -this->x, -this->y ); }

		T operator*(const Vec2 &v) const
		{
			return this->x * v.x + this->y * v.y;
		}

		Vec2 operator*(const T f) const
		{
			return Vec2(this->x * f, this->y * f);
		}

		Vec2 operator/(const T f) const
		{
			return Vec2(this->x / f, this->y / f);
		}

		Vec2 operator+(const Vec2 &v) const
		{
			return Vec2(this->x + v.x, this->y + v.y);
		}

		Vec2 operator+(const T offset) const
		{
			return Vec2(this->x + offset, this->y + offset);
		}

		Vec2 operator-(const Vec2 &v) const
		{
			return Vec2(this->x - v.x, this->y - v.y);
		}

		Vec2 operator-(const T offset) const
		{
			return Vec2(this->x - offset, this->y - offset);
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

		friend Vec2 operator*(const T f, const Vec2 &a)
		{
			return Vec2(f * a.x, f * a.y);
		}

		friend bool operator==(const Vec2 &a, const Vec2 &b)
		{
			return a.x == b.x && a.y == b.y;
		}
	};

	template<typename T>
	const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Down(0, -1);

	template<typename T>
	const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Left(-1, 0);

	template<typename T>
	const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::One(1, 1);

	template<typename T>
	const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Right(1, 0);

	template<typename T>
	const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Up(0, 1);

	template<typename T>
	const Vec2<T, Arithmetic<T>> Vec2<T, Arithmetic<T>>::Zero;
}

using Vec2f = rainbow::Vec2<float>;
using Vec2i = rainbow::Vec2<int>;
using Vec2u = rainbow::Vec2<unsigned>;

#endif
