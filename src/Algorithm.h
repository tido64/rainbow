/// Provides algorithms and mathematical functions.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <cfloat>
#include <cmath>

#include "Random/Mersenne_twister.h"

namespace Rainbow
{
	/// Angle between two points with bearing north.
	inline float angle(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		return atanf((b_y - a_y) / (b_x - a_x));
	}

	/// Convert degrees to radians.
	inline float deg2rad(const float d) { return d * 0.017453292519943; }

	/// Compare two floating point numbers and approximate.
	/// \return True when approximately equal
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	/// Get a random number.
	inline float mt_random() { return Random::Instance().next(); }

	/// Convert radians to degrees.
	inline float rad2deg(const float r) { return r * 57.295779513082325; }
}

#endif
