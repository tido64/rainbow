// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_RANDOM_H_
#define COMMON_RANDOM_H_

#include "Common/Chrono.h"
#include "Common/Constraints.h"
#include "Common/Logging.h"

#define DSFMT_MEXP 19937
#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(_M_X64)
#	define HAVE_SSE2
#elif defined(__ALTIVEC__)
#	define HAVE_ALTIVEC
#endif
#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif
#include <dSFMT/dSFMT.h>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

/// C++ wrapper for dSFMT random number generator.
namespace Random
{
	/// Returns the next generated random number in [0, 1).
	inline double next();

	/// Returns the next generated random number in [0, n).
	template<typename T, typename Enable>
	T next(const T n);

	/// Returns the next generated random number in [n1, n2).
	template<typename T, typename Enable>
	T next(const T n1, const T n2);

	/// Sets the random number generator seed. Must be called before any other
	/// calls.
	inline void seed(const uint32_t seed);

	double next()
	{
		return dsfmt_gv_genrand_close_open();
	}

	template<typename T, typename = Arithmetic<T>>
	T next(const T n)
	{
		return static_cast<T>(next() * n);
	}

	template<typename T, typename = Arithmetic<T>>
	T next(const T n1, const T n2)
	{
		R_ASSERT(n1 < n2, "Parameters must be in ascending order");
		return static_cast<T>(next() * (n2 - n1) + n1);
	}

	void seed(const uint32_t seed)
	{
		dsfmt_gv_init_gen_rand(
		    (seed == 0) ? Chrono::clock::now().time_since_epoch().count()
		                : seed);
	}
}

#endif
