// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Chrono.h"

using rainbow::Chrono;

TEST(ChronoTest, TimeDifferenceBetweenUpdates)
{
    const Chrono::duration::rep threshold = 20;
    const Chrono::duration::rep times[]{16, 1000, 0};

    Chrono chrono;
    ASSERT_EQ(chrono.delta(), 0);

    for (int i = 0; times[i] > 0; ++i)
    {
        Chrono::sleep(times[i]);
        chrono.tick();
        ASSERT_LE(times[i], chrono.delta());
        ASSERT_GE(times[i] + threshold, chrono.delta());
    }
}
