/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Common/Chrono.h"

#if defined(RAINBOW_UNIX) && !defined(RAINBOW_SDL)

unsigned long Chrono::get_time()
{
#if _POSIX_TIMERS <= 0

	timeval t;
	gettimeofday(&t, nullptr);
	return t.tv_sec * 1000 + t.tv_usec / 1000;

#else

	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1e6;

#endif
}

#endif
