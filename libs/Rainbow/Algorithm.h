/// An algorithms name space for often used functions.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <cfloat>
#include <cmath>

#include <Rainbow/Random/Mersenne_twister.h>

namespace Rainbow
{
	namespace Animation
	{
		static const double third = 1.0 / 3.0;
		static const double quarter = 0.25;

		inline float ease_in_cubic(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * t * t * t;
		}

		inline float ease_in_quad(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * t * t * t * t;
		}

		inline float ease_in_square(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * t * t;
		}

		inline float ease_out_cubic(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * pow(t, third);
		}

		inline float ease_out_quad(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * pow(t, quarter);
		}

		inline float ease_out_square(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * sqrt(t);
		}
	}

	/// Returns the distance between two points.
	/// \param a_x  Point A's x-coordinate
	/// \param a_y  Point A's y-coordinate
	/// \param b_x  Point B's x-coordinate
	/// \param b_y  Point B's y-coordinate
	/*static float distance(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		float s1 = b_x - a_x;
		float s2 = b_y - a_y;
		s1 *= s1;
		s2 *= s2;
		return sqrt(s1 + s2);
	}*/

	/// Compares two floating point numbers. Returns true on approximately equal.
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	inline float mt_random()
	{
		static Random *rand_gen = Random::Instance();
		return rand_gen->next();
	}
}

#endif
