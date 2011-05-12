#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "Common/Constants.h"
#include "Drawable.h"

/// A generic sprite animation.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Animation : public Drawable
{
public:
	bool stopped;  ///< Whether this clip has stopped

	Animation() : stopped(true), frames(0) { }

	virtual void play() = 0;
	virtual void step() = 0;

protected:
	unsigned int frames;

	float ease_in_cubic(const float start, const float end, const float t);
	float ease_in_quad(const float start, const float end, const float t);
	float ease_in_square(const float start, const float end, const float t);
	float ease_out_cubic(const float start, const float end, const float t);
	float ease_out_quad(const float start, const float end, const float t);
	float ease_out_square(const float start, const float end, const float t);
};

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
	return start + (end - start) * pow(t, kThird);
}

inline float Animation::ease_out_quad(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * pow(t, kQuarter);
}

inline float Animation::ease_out_square(const float start, const float end, const float t)
{
	assert(t >= 0.0f && t < 1.0f);
	return start + (end - start) * sqrt(t);
}

#endif
