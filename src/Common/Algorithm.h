/// Provides algorithms and mathematical methods.
///
/// \see http://graphics.stanford.edu/~seander/bithacks.html
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <cmath>
#include <cfloat>
#include <cstdlib>

#include "Common/Constants.h"

namespace Rainbow
{
	/// Convert radians to degrees.
	inline float degrees(const float r)
	{
		return r * kRadian;
	}

	/// Compare two floating point numbers and approximate.
	/// \return \c true when approximately equal.
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	/// Determine whether an integer is a power of 2.
	inline bool is_pow2(const unsigned int i)
	{
		return i && !(i & (i - 1));
	}

	template<typename T>
	inline const T& max(const T &a, const T &b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	inline const T& min(const T &a, const T &b)
	{
		return (b < a) ? b : a;
	}

	/// Calculate the next power of 2.
	/// \note 0 is incorrectly considered a power of 2.
	inline unsigned int next_pow2(unsigned int i)
	{
		--i;
		i |= i >>  1;
		i |= i >>  2;
		i |= i >>  4;
		i |= i >>  8;
		i |= i >> 16;
		return ++i;
	}

	/// Convert degrees to radians.
	inline float radians(const float d)
	{
		return d * kDegree;
	}

	template<typename T>
	inline void swap(T &a, T &b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	/// Convert a UTF-8 character to UTF-32.
	/// \param[in]  str    UTF-8 encoded string.
	/// \param[out] bytes  Number of bytes consumed.
	/// \return UTF-32 character. Otherwise -1.
	inline unsigned long utf8_decode(const unsigned char *str, size_t &bytes)
	{
		bytes = 0;
		if (!(str[0] & 0x80))
		{
			bytes = 1;
			return str[0];
		}
		else if ((str[0] & 0xe0) == 0xc0)
		{
			if ((str[1] & 0xc0) != 0x80)
				return -1;

			bytes = 2;
			unsigned long ucs = str[0] & 0x1f;
			ucs <<= 6;
			ucs |= str[1] & 0x3f;
			return ucs;
		}
		else if ((str[0] & 0xf0) == 0xe0)
		{
			if ((str[1] & 0xc0) != 0x80 || (str[2] & 0xc0) != 0x80)
				return -1;

			bytes = 3;
			unsigned long ucs = str[0] & 0x0f;
			ucs <<= 6;
			ucs |= str[1] & 0x3f;
			ucs <<= 6;
			ucs |= str[2] & 0x3f;
			return ucs;
		}
		else if ((str[0] & 0xf8) == 0xf0)
		{
			if ((str[1] & 0xc0) != 0x80 || (str[2] & 0xc0) != 0x80 || (str[3] & 0xc0) != 0x80)
				return -1;

			bytes = 4;
			unsigned long ucs = str[0] & 0x0e;
			ucs <<= 6;
			ucs |= str[1] & 0x3f;
			ucs <<= 6;
			ucs |= str[2] & 0x3f;
			ucs <<= 6;
			ucs |= str[3] & 0x3f;
			return ucs;
		}
		else
			return -1;
	}
}

#endif
