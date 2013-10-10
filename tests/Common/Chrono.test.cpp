#include "Common/Chrono.h"

TEST(ChronoTest, Delta)
{
	const Chrono::duration::rep threshold = 20;
	const Chrono::duration::rep times[] = { 16, 1000, 0 };

	Chrono chrono;
	for (int i = 0; times[i] > 0; ++i)
	{
		Chrono::sleep(times[i]);
		chrono.update();
		ASSERT_LE(times[i], chrono.delta());
		ASSERT_GT(times[i] + threshold, chrono.delta());
	}
}
