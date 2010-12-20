/// A math helper class.

/// The reason this file can't be named Math.h is because Xcode can't
/// distinguish between this file and another file placed elsewhere.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef MATH_H_
#define MATH_H_

#include <cfloat>
#include <cmath>

#include <Rainbow/Random/Mersenne_twister.h>

namespace Ars
{
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
