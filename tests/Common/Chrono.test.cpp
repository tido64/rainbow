#include <unistd.h>
#include "Common/Chrono.h"

TEST(ChronoTest, DISABLED_Sleep)
{
	Chrono chrono;
	chrono.update();
	usleep(16000);
	chrono.update();
	ASSERT_FLOAT_EQ(16.0f, static_cast<const float>(chrono.diff()));
}
