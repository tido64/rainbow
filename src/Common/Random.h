// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_RANDOM_H_
#define COMMON_RANDOM_H_

#include <cstdint>
#include <random>

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

#include "Common/Constraints.h"
#include "Common/Logging.h"

namespace rainbow
{
	/// <summary>C++ wrapper for dSFMT pseudorandom number generator.</summary>
	template<typename T, typename = EnableIfFloatingPoint<T>>
	struct dSFMT
	{
		void seed(const uint32_t seed = 0)
		{
			dsfmt_gv_init_gen_rand(seed == 0 ? std::random_device{}() : seed);
		}

		T operator()() { return dsfmt_gv_genrand_close_open(); }

		template<typename N>
		N operator()(const N n)
		{
			return static_cast<N>(dsfmt_gv_genrand_close_open() * n);
		}

		template<typename N>
		N operator()(const N m, const N n)
		{
			R_ASSERT(m < n, "Parameters must be in ascending order");
			return static_cast<N>(dsfmt_gv_genrand_close_open() * (n - m) + m);
		}
	};

	extern dSFMT<float> random;
}

#endif
