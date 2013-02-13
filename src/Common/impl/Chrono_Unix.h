#if defined(RAINBOW_ANDROID) || (defined(RAINBOW_UNIX) && !defined(RAINBOW_MAC))
#include <ctime>
#include <unistd.h>

namespace Rainbow
{
	/// Unix-implementation of 'get_time' operator.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	struct GetTimeUnix
	{
		unsigned long operator()() const
		{
			timespec t;
			clock_gettime(CLOCK_MONOTONIC, &t);
			return t.tv_sec * 1000 + t.tv_nsec / 1e6;
		}
	};
}

typedef Rainbow::ChronoBase<Rainbow::GetTimeUnix> Chrono;

#endif
