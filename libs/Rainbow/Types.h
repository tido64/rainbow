#ifndef TYPES_H_
#define TYPES_H_

/// Structure for storing a color (RGBA).
/// FIXME: Different default values for char <-> float

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<typename T>
struct __Color4
{
	T r, g, b, a;

	__Color4() : r(1), g(1), b(1), a(1) { }
	__Color4(const T r, const T g, const T b, const T a = 1) :
		r(r), g(g), b(b), a(a) { }
};


/// Structure for storing a pair of numerical values.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<typename T>
struct __Vec2
{
	T x, y;

	__Vec2() : x(0), y(0) { }
	__Vec2(const T x, const T y) : x(x), y(y) { }

	/// Returns the angle (in radians) between two points.
	inline float angle(const __Vec2<T> &v) const
	{
		return atan2f(v.y - this->y, v.x - this->x);
	}

	/// Returns the dot product of two vectors
	inline T dot(const __Vec2<T> &v) const
	{
		return this->x * this->y + v.x * v.y;
	}

	/// Returns the distance between two points.
	T distance(const __Vec2<T> &v) const
	{
		T dx = v.x - this->x;
		T dy = v.y - this->y;
		return sqrt(dx * dx + dy * dy);
	}

	/// Determines whether the vector is zero.
	inline bool is_zero() const
	{
		return (this->x == 0) && (this->y == 0);
	}

	/// Zeroes the vector.
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

typedef __Color4<unsigned char> Color4ub;
typedef __Color4<float> Color4f;
typedef __Vec2<float> Vec2f;
typedef __Vec2<int> Vec2i;


/// A typical sprite vertex contains x- and y-coordinates for position and texture.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
struct SpriteVertex
{
	Vec2f position;
	Vec2f texcoord;
};

#endif
