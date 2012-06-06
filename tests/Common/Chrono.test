#include <unistd.h>
#include "Common/Chrono.h"

TEST(ChronoTest, DISABLED_Sleep)
{
	Chrono::Instance().update();
	usleep(16000);
	Chrono::Instance().update();
	ASSERT_FLOAT_EQ(16.0f, static_cast<const float>(Chrono::Instance().diff()));
}
