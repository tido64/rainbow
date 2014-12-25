// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef RAINBOW_TEST

#include "Tests/Tests.h"

#define CATCH_CONFIG_RUNNER
#include <Catch/single_include/catch.hpp>

#include "Tests/Common/Algorithm.test.cc"
#include "Tests/Common/Chrono.test.cc"
#include "Tests/Common/Color.test.cc"
#include "Tests/Common/Data.test.cc"
#include "Tests/Common/Link.test.cc"
#include "Tests/Common/SharedPtr.test.cc"
#include "Tests/Common/TimedEvent.test.cc"
#include "Tests/Common/TreeNode.test.cc"
#include "Tests/Common/Vec2.test.cc"
#include "Tests/Common/Vec3.test.cc"
#include "Tests/ConFuoco/ConFuoco.test.cc"
#include "Tests/FileSystem/Path.test.cc"
#include "Tests/Input/Input.test.cc"
#include "Tests/Input/Touch.test.cc"

namespace Rainbow
{
	int run_tests(int argc, char *argv[])
	{
		return Catch::Session().run(argc, argv);
	}
}

#endif
