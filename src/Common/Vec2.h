#ifndef VEC2_H_
#define VEC2_H_

#include <cmath>

namespace Rainbow
{
	/// Structure for storing a two-dimensional vector.
	///
	/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	template<typename T>
	struct Vec2
	{
		T x, y;

		Vec2() : x(0), y(0) { }
		Vec2(const T x, const T y) : x(x), y(y) { }

		/// Return the angle (in radians) between two points.
		float angle(const Vec2<T> &v) const
		{
			return atan2f(v.y - this->y, v.x - this->x);
		}

		/// Return the distance between two points.
		T distance(const Vec2<T> &v) const
		{
			T dx = v.x - this->x;
			T dy = v.y - this->y;
			return sqrt(dx * dx + dy * dy);
		}

		/// Return the dot product of two vectors
		T dot(const Vec2<T> &v) const
		{
			return this->x * v.x + this->y * v.y;
		}

		/// Determine whether the vector is zero.
		bool is_zero() const
		{
			return !this->x && !this->y;
		}

		/// Zero the vector.
		void zero()
		{
			this->x = 0;
			this->y = 0;
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
}

typedef Rainbow::Vec2<float> Vec2f;
typedef Rainbow::Vec2<int> Vec2i;

#endif
