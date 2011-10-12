#ifndef CHRONO_H_
#define CHRONO_H_

#include <cassert>
#include <ctime>
//#include <unistd.h>

/// Simple class for keeping time.
///
/// Resolution of time is in milliseconds.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Chrono
{
public:
	/// Return current time.
	static unsigned long current()
	{
		assert(tm_current > 0);
		return tm_current;
	}

	/// Return last frame's time.
	static unsigned long last_frame()
	{
		assert(tm_last > 0);
		return tm_last;
	}

	/// Tick tock.
	static void update()
	{
		tm_last = tm_current;
		timespec t;
		clock_gettime(CLOCK_MONOTONIC, &t);
		tm_current = t.tv_sec * 1000 + t.tv_nsec / 1000;
	}

private:
	static unsigned long tm_current;  ///< Current frame time in milliseconds
	static unsigned long tm_last;     ///< Last frame time in milliseconds
};

#endif
