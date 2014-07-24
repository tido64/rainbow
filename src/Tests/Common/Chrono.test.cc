// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Chrono.h"

// TODO: The following newlines were added for Catch to create unique names.

TEST_CASE("Time difference between updates", "[chrono]")
{
	const Chrono::duration::rep threshold = 20;
	const Chrono::duration::rep times[] = { 16, 1000, 0 };

	Chrono chrono;
	for (int i = 0; times[i] > 0; ++i)
	{
		Chrono::sleep(times[i]);
		chrono.update();
		REQUIRE(chrono.delta() >= times[i]);
		REQUIRE(chrono.delta() < times[i] + threshold);
	}
}
