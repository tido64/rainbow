// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Chrono.h"

#ifdef RAINBOW_ANDROID
#include <ctime>
#include <unistd.h>

unsigned long Chrono::get_time()
{
	timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1e6;
}

#endif
