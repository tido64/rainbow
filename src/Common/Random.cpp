#include "Random.h"

#ifdef RAINBOW_UNIX
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif

#ifndef RAINBOW_IOS
#	include <dSFMT/dSFMT.c>
#endif

#ifdef RAINBOW_UNIX
#	pragma GCC diagnostic pop
#endif

Random::Random(uint32_t seed)
{
	if (seed == 0)
		seed = static_cast<uint32_t>(time(nullptr));
	dsfmt_init_gen_rand(&dsfmt_global_data, seed);
}
