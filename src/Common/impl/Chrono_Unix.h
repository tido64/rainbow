#if defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX)
#include <ctime>

#ifdef RAINBOW_MAC
#	include <mach/mach_time.h>
#endif

namespace
{
	const time_t kMilliseconds = 1000;
}

void Chrono::sleep(const unsigned int ms)
{
	const time_t sec = ms / kMilliseconds;
	const time_t msec = ms - sec * kMilliseconds;
	const timespec request = { sec, msec * kMilliseconds * kMilliseconds };
	nanosleep(&request, nullptr);
}

unsigned long Chrono::time()
{
#ifdef RAINBOW_MAC
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	const uint64_t t = mach_absolute_time();
	return (double)t * (double)info.numer / (double)(info.denom * 1.0e6);
#else
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * kMilliseconds + t.tv_nsec / 1e6;
#endif
}

#endif
