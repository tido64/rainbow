#if defined(RAINBOW_ANDROID) || (defined(RAINBOW_UNIX) && !defined(RAINBOW_MAC))
#include <ctime>

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

	template<class T>
	void ChronoBase<T>::sleep(const unsigned int ms)
	{
		const struct timespec request = { 0, ms * 1000000l };
		nanosleep(&request, nullptr);
	}
}

typedef Rainbow::ChronoBase<Rainbow::GetTimeUnix> Chrono;

#endif
