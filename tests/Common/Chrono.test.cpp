#include "Common/Chrono.h"

TEST(ChronoTest, Diff)
{
	Chrono chrono;
	chrono.update();
	Chrono::sleep(16);
	chrono.update();
	ASSERT_LE(16.0f, chrono.diff());
}
