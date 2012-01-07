// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Chrono.h"

#ifdef RAINBOW_IOS
#include <mach/mach_time.h>

unsigned long Chrono::get_time()
{
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	const uint64_t t = mach_absolute_time();
	return (double)t * (double)info.numer / (double)info.denom / 1.0e6;
}

void Chrono::update(const unsigned int t)
{
	this->tm_previous = this->tm_current;
	this->tm_current += t;
	this->tm_dt = this->tm_current - this->tm_previous;
}

#endif
