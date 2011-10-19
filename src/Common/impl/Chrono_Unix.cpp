/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Platform.h"
#ifdef RAINBOW_UNIX
#include <unistd.h>
#if _POSIX_TIMERS <= 0
#	warning "POSIX Realtime extension not found."
#	include <sys/time.h>
#else
#	include <ctime>
#endif
#include "Common/Chrono.h"

Chrono::Chrono() : tm_current(0), tm_last(0) { }

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
