/// A generic sprite animation.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "Drawable.h"

class Animation : public Drawable
{
public:
	bool stopped;  ///< Whether this clip has stopped

	Animation() : stopped(true), frames(0) { }

	virtual void play() = 0;
	virtual void step() = 0;

protected:
	unsigned int frames;

	static const double third;
	static const double quarter;

	float ease_in_cubic(const float start, const float end, const float t);
	float ease_in_quad(const float start, const float end, const float t);
	float ease_in_square(const float start, const float end, const float t);
	float ease_out_cubic(const float start, const float end, const float t);
	float ease_out_quad(const float start, const float end, const float t);
	float ease_out_square(const float start, const float end, const float t);
};

const double Animation::third = 1.0 / 3.0;
const double Animation::quarter = 0.25;

inline float Animation::ease_in_cubic(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * t * t * t;
}

inline float Animation::ease_in_quad(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * t * t * t * t;
}

inline float Animation::ease_in_square(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * t * t;
}

inline float Animation::ease_out_cubic(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * pow(t, third);
}

inline float Animation::ease_out_quad(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * pow(t, quarter);
}

inline float Animation::ease_out_square(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * sqrt(t);
}

#endif
