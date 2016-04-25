// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Chrono.h"

TEST(ChronoTest, TimeDifferenceBetweenUpdates)
{
    const Chrono::duration::rep threshold = 20;
    const Chrono::duration::rep times[]{16, 1000, 0};

    Chrono chrono;
    ASSERT_EQ(0, chrono.delta());

    for (int i = 0; times[i] > 0; ++i)
    {
        Chrono::sleep(times[i]);
        chrono.tick();
        ASSERT_LE(times[i], chrono.delta());
        ASSERT_GE(times[i] + threshold, chrono.delta());
    }
}
