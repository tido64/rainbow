#include "Platform.h"
#include "Random.h"

#if defined(__clang__) || defined(__GNUC__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif
#include <dSFMT/dSFMT.c>
#if defined(__clang__) || defined(__GNUC__)
#	pragma GCC diagnostic pop
#endif

Random::Random(uint32_t seed)
{
	if (seed == 0)
		seed = static_cast<uint32_t>(time(nullptr));
	dsfmt_init_gen_rand(&dsfmt_global_data, seed);
}
