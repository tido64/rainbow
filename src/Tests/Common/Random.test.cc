// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/Algorithm.h"
#include "Common/Random.h"

namespace
{
	// dSFMT2-19937:117-19:ffafffffffb3f-ffdfffc90fffd
	// init_gen_rand(0) generated randoms [0, 1)
	const double kKnownRandoms[]{0.030581026769374, 0.213140320067012,
	                             0.299002525016001, 0.381138853044628,
	                             0.863488397063594, 0.133443440024236,
	                             0.073465290616508, 0.924735636004535};
}

TEST_CASE("Randoms in range [0, 1)", "[random]")
{
	dsfmt_gv_init_gen_rand(0);

	for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
		REQUIRE(Approx(rainbow::random()) == kKnownRandoms[i]);
}

TEST_CASE("Randoms in range [0, n)", "[random]")
{
	const double upper = 8;
	dsfmt_gv_init_gen_rand(0);

	for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
		REQUIRE(Approx(rainbow::random(upper)) == kKnownRandoms[i] * upper);
}

TEST_CASE("Randoms in range [m, n)", "[random]")
{
	const double lower = 8.0;
	const double upper = 16.0;
	dsfmt_gv_init_gen_rand(0);

	for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
	{
		REQUIRE(Approx(rainbow::random(lower, upper)) ==
		        kKnownRandoms[i] * lower + lower);
	}
}

TEST_CASE("Seeded randoms in range [0, 1)", "[random]")
{
	rainbow::random.seed(1);
	for (size_t i = 0; i < rainbow::array_size(kKnownRandoms); ++i)
		REQUIRE(Approx(rainbow::random()) != kKnownRandoms[i]);
}
