#ifndef VEC2_H_
#define VEC2_H_

namespace Rainbow
{
	/// Structure for storing a two-dimensional vector.

	/// Copyright 2010-11 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	template<typename T>
	struct __Vec2
	{
		T x, y;

		__Vec2() : x(0), y(0) { }
		__Vec2(const T x, const T y) : x(x), y(y) { }

		/// Return the angle (in radians) between two points.
		inline float angle(const __Vec2<T> &v) const
		{
			return atan2f(v.y - this->y, v.x - this->x);
		}

		/// Return the dot product of two vectors
		inline T dot(const __Vec2<T> &v) const
		{
			return this->x * this->y + v.x * v.y;
		}

		/// Return the distance between two points.
		T distance(const __Vec2<T> &v) const
		{
			T dx = v.x - this->x;
			T dy = v.y - this->y;
			return sqrt(dx * dx + dy * dy);
		}

		/// Determine whether the vector is zero.
		inline bool is_zero() const
		{
			return (this->x == 0) && (this->y == 0);
		}

		/// Zero the vector.
		inline void zero()
		{
			this->x = 0;
			this->y = 0;
		}

		__Vec2<T>& operator+=(const __Vec2<T> &v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}

		__Vec2<T>& operator-=(const __Vec2<T> &v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}

		__Vec2<T>& operator*=(const T &f)
		{
			this->x *= f;
			this->y *= f;
			return *this;
		}
	};
}

typedef Rainbow::__Vec2<float> Vec2f;
typedef Rainbow::__Vec2<int> Vec2i;

#endif
