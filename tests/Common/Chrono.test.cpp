#include "Common/Chrono.h"

TEST(ChronoTest, Diff)
{
	const unsigned long threshold = 20;
	const unsigned long times[] = { 16, 1000, 0 };

	Chrono chrono;
	chrono.update();
	for (int i = 0; times[i] > 0; ++i)
	{
		Chrono::sleep(times[i]);
		chrono.update();
		ASSERT_LE(times[i], chrono.diff());
		ASSERT_GT(times[i] + threshold, chrono.diff());
	}
}
