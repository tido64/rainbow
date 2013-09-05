/// Provides algorithms and mathematical methods.
///
/// \see http://graphics.stanford.edu/~seander/bithacks.html
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef COMMON_ALGORITHM_H_
#define COMMON_ALGORITHM_H_

#include <cmath>
#include <cstdlib>
#include <limits>

#include "Common/Constants.h"
#include "Common/Functional.h"

#ifdef _MSC_VER
#	undef max
#	undef min
#endif

namespace Rainbow
{
	namespace
	{
		// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
		// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

		const unsigned int kUTF8Accept = 0;
		const unsigned int kUTF8Reject = 12;

		const unsigned char kUTF8DecoderTable[] = {
			// The first part of the table maps bytes to character classes that
			// to reduce the size of the transition table and create bitmasks.
			 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
			 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
			 8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

			// The second part is a transition table that maps a combination
			// of a state of the automaton and a character class to a state.
			 0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
			12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
			12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
			12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
			12,36,12,12,12,12,12,12,12,12,12,12,
		};
	}

	struct utf_t
	{
		unsigned int code;
		unsigned int bytes;

		utf_t() : code(0), bytes(0) { }

		operator unsigned int() const
		{
			return this->code;
		}
	};

	/// Converts radians to degrees.
	inline float degrees(const float r) pure;

	/// Compares two floating point numbers and approximate.
	/// \return \c true when approximately equal.
	template<typename T>
	inline bool equal(const T &a, const T &b) pure;

	/// Fast inverse square root by 0x5f3759df.
	inline float fast_invsqrt(float x) pure;

	/// Determines whether an integer is a power of 2.
	inline bool is_pow2(const unsigned int i) pure;

	/// Low-pass filter.
	inline float low_pass(const float value, const float low_pass) pure;

	template<typename T>
	inline const T& max(const T &a, const T &b) pure;

	template<typename T>
	inline const T& min(const T &a, const T &b) pure;

	/// Calculates the next power of 2.
	/// \note 0 is incorrectly considered a power of 2.
	inline unsigned int next_pow2(unsigned int i) pure;

	/// Converts degrees to radians.
	inline float radians(const float d) pure;

	/// Converts a UTF-8 character to UTF-32.
	inline utf_t utf8_decode(const unsigned char *str) pure;

	float degrees(const float r)
	{
		return r * kRadian;
	}

	template<typename T>
	bool equal(const T &a, const T &b)
	{
		return fabs(a - b) <= max(fabs(a), fabs(b)) * std::numeric_limits<T>::epsilon();
	}

	float fast_invsqrt(float x)
	{
		float xhalf = x * 0.5f;
		int i = *reinterpret_cast<int*>(&x);
		i = 0x5f3759df - (i >> 1);
		x = *reinterpret_cast<float*>(&i);
		return x * (1.5f - (xhalf * x * x));
	}

	bool is_pow2(const unsigned int i)
	{
		return i && !(i & (i - 1));
	}

	float low_pass(const float value, const float low_pass)
	{
		return kLowPassAlpha * powf(10.0f, value * kLowPassAlpha) + (1.0f - kLowPassAlpha) * low_pass;
	}

	template<typename T>
	const T& max(const T &a, const T &b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	const T& min(const T &a, const T &b)
	{
		return (b < a) ? b : a;
	}

	unsigned int next_pow2(unsigned int i)
	{
		--i;
		i |= i >>  1;
		i |= i >>  2;
		i |= i >>  4;
		i |= i >>  8;
		i |= i >> 16;
		return ++i;
	}

	float radians(const float d)
	{
		return d * kDegree;
	}

	utf_t utf8_decode(const unsigned char *str)
	{
		unsigned int state = kUTF8Accept;
		utf_t c;
		do
		{
			const unsigned int type = kUTF8DecoderTable[*str];
			c.code = (state != kUTF8Accept)
					? (*str & 0x3fu) | (c.code << 6)
					: (0xff >> type) & (*str);
			state = kUTF8DecoderTable[256 + state + type];
			if (state == kUTF8Reject)
				return utf_t();
			++c.bytes;
			++str;
		} while (state != kUTF8Accept);
		return c;
	}
}

#endif
