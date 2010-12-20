/// Various structs used throughout.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef TYPES_H_
#define TYPES_H_

struct Color4f
{
	float r, g, b, a;

	Color4f() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }
	Color4f(const float r, const float g, const float b, const float a = 1.0f) :
		r(r), g(g), b(b), a(a)
	{ }
};

struct Vec2f
{
	float x, y;

	Vec2f() : x(0.0f), y(0.0f) { }
	Vec2f(const float x, const float y) : x(x), y(y) { }

	inline float dot(const Vec2f &v)
	{
		return this->x * this->y + v.x * v.y;
	}

	void operator += (const Vec2f &v)
	{
		this->x += v.x;
		this->y += v.y;
	}

	void operator -= (const Vec2f &v)
	{
		this->x -= v.x;
		this->y -= v.y;
	}

	void operator *= (const float &f)
	{
		this->x *= f;
		this->y *= f;
	}
};

struct ParticleVertex
{
	Vec2f position;
	Vec2f texcoord;
	Color4f color;
};

struct SpriteVertex
{
	Vec2f position;
	Vec2f texcoord;
};

#endif
