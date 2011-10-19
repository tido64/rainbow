/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Platform.h"
#ifdef RAINBOW_IOS
#include <mach/mach_time.h>
#include "Common/Chrono.h"

Chrono::Chrono() : tm_current(0), tm_last(0) { }

unsigned long Chrono::get_time()
{
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	const uint64_t t = mach_absolute_time();
	return (double)t * (double)info.numer / (double)info.denom / 1.0e6;
}

#endif
