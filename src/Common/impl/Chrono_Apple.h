#ifdef RAINBOW_MAC
#include <ctime>
#include <mach/mach_time.h>

namespace Rainbow
{
	/// Apple-implementation of 'get_time' operator.
	///
	/// This method is compatible with iOS as well but we don't need it there
	/// because we're using GLKit.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	struct GetTimeApple
	{
		unsigned long operator()() const
		{
			mach_timebase_info_data_t info;
			mach_timebase_info(&info);
			const uint64_t t = mach_absolute_time();
			return (double)t * (double)info.numer / (double)(info.denom * 1.0e6);
		}
	};

	template<class T>
	void ChronoBase<T>::sleep(const unsigned int ms)
	{
		const struct timespec request = { 0, ms * 1000000l };
		nanosleep(&request, nullptr);
	}
}

typedef Rainbow::ChronoBase<Rainbow::GetTimeApple> Chrono;

#endif
