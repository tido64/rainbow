/// Provides algorithms and mathematical functions.

/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <cfloat>
#include <cmath>

#include "Common/Constants.h"
#include "Random/Mersenne_twister.h"

namespace Rainbow
{
	/// Angle between two points with bearing north.
	inline float angle(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		return atanf((b_y - a_y) / (b_x - a_x));
	}

	/// Convert degrees to radians.
	inline float deg2rad(const float d) { return d * kPi_180; }

	/// Calculate the distance between two points.
	inline float distance(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		float s1 = b_x - a_x;
		float s2 = b_y - a_y;
		s1 *= s1;
		s2 *= s2;
		return sqrt(s1 + s2);
	}

	/// Compare two floating point numbers and approximate.
	/// \return True when approximately equal
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	/// Get a random number.
	inline float mt_random() { return static_cast<float>(Random::Instance().next()); }

	/// Convert radians to degrees.
	inline float rad2deg(const float r) { return r * k180_Pi; }
}

#endif
