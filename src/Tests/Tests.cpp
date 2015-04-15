// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef RAINBOW_TEST

#include "Tests/Tests.h"

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

namespace rainbow
{
	int run_tests(int argc, char *argv[])
	{
		return Catch::Session().run(argc, argv);
	}
}

#endif
