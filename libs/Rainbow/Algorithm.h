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
	/// Compares two floating point numbers. Returns true on approximately equal.
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	inline float mt_random()
	{
		return Random::Instance().next();
	}
}

#endif
