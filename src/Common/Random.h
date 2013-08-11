#ifndef COMMON_RANDOM_H_
#define COMMON_RANDOM_H_

#include <ctime>

#include "Common/Debug.h"

#if defined(RAINBOW_UNIX)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif

#define DSFMT_MEXP 19937
#include <dSFMT/dSFMT.h>

#if defined(RAINBOW_UNIX)
#	pragma GCC diagnostic pop
#endif

/// C++ wrapper for dSFMT random number generator.
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
namespace Random
{
	/// Returns the next generated random number in [0, 1).
	inline double next();

	/// Returns the next generated random number in [0, n).
	template<class T>
	inline T next(const T &n);

	/// Returns the next generated random number in [n1, n2).
	template<class T>
	inline T next(const T &n1, const T &n2);

	/// Sets the random number generator seed. Must be called before any other calls.
	inline void seed(const uint32_t seed);

	double next()
	{
		return dsfmt_gv_genrand_close_open();
	}

	template<class T>
	T next(const T &n)
	{
		return static_cast<T>(next() * n);
	}

	template<class T>
	T next(const T &n1, const T &n2)
	{
		R_ASSERT(n1 < n2, "Parameters must be in ascending order");
		return static_cast<T>(next() * (n2 - n1) + n1);
	}

	void seed(const uint32_t seed)
	{
		dsfmt_gv_init_gen_rand((seed == 0) ? static_cast<uint32_t>(time(nullptr)) : seed);
	}
}

#endif
