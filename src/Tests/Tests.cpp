// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Tests/Tests.h"

#include <gtest/gtest.h>

#include "Common/Random.h"

namespace rainbow
{
    auto run_tests(int argc, char* argv[]) -> int
    {
        random.seed();
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
}
