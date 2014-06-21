// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VEC2_H_
#define COMMON_VEC2_H_

#include "Common/Algorithm.h"

namespace Rainbow
{
	/// Structure for storing a two-dimensional vector.
	template<typename T>
	struct Vec2
	{
		union
		{
			T x;
			T width;
		};

		union
		{
			T y;
			T height;
		};

		Vec2() : x(0), y(0) { }
		Vec2(const T x, const T y) : x(x), y(y) { }

		/// Returns the angle (in radians) between two points.
		float angle(const Vec2<T> &v) const
		{
			return atan2f(v.y - this->y, v.x - this->x);
		}

		/// Returns the distance between two points.
		float distance(const Vec2<T> &v) const
		{
			T dx = v.x - this->x;
			T dy = v.y - this->y;
			return sqrt(dx * dx + dy * dy);
		}

		/// Returns the dot product of two vectors
		T dot(const Vec2<T> &v) const
		{
			return this->x * v.x + this->y * v.y;
		}

		/// Returns whether the vector is zero.
		bool is_zero() const
		{
			return !this->x && !this->y;
		}

		bool operator==(const Vec2<T> &v) const
		{
			return this->x == v.x && this->y == v.y;
		}

		Vec2<T> operator+(const T offset)
		{
			return Vec2<T>(this->x + offset, this->y + offset);
		}

		Vec2<T>& operator+=(const Vec2<T> &v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}

		Vec2<T>& operator-=(const Vec2<T> &v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}

		Vec2<T>& operator*=(const T &f)
		{
			this->x *= f;
			this->y *= f;
			return *this;
		}
	};

	template<>
	inline bool Vec2<float>::is_zero() const
	{
		return isequal(0.0f, this->x) && isequal(0.0f, this->y);
	}
}

typedef Rainbow::Vec2<float> Vec2f;
typedef Rainbow::Vec2<int> Vec2i;

#endif
