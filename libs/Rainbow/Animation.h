/// Animation helper functions.

/// \see http://www.fooplot.com/
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <cassert>
#include <cmath>

struct Animation
{
	static const double third = 1.0 / 3.0;
	static const double quarter = 0.25;

	static float ease_in_cubic(const float start, const float end, const float t)
	{
		assert(t >= 0.0f && t < 1.0f);
		return start + (end - start) * t * t * t;
	}

	static float ease_in_quad(const float start, const float end, const float t)
	{
		assert(t >= 0.0f && t < 1.0f);
		return start + (end - start) * t * t * t * t;
	}

	static float ease_in_square(const float start, const float end, const float t)
	{
		assert(t >= 0.0f && t < 1.0f);
		return start + (end - start) * t * t;
	}

	static float ease_out_cubic(const float start, const float end, const float t)
	{
		assert(t >= 0.0f && t < 1.0f);
		return start + (end - start) * pow(t, third);
	}

	static float ease_out_quad(const float start, const float end, const float t)
	{
		assert(t >= 0.0f && t < 1.0f);
		return start + (end - start) * pow(t, quarter);
	}

	static float ease_out_square(const float start, const float end, const float t)
	{
		assert(t >= 0.0f && t < 1.0f);
		return start + (end - start) * sqrt(t);
	}
};

#endif
