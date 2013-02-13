#ifdef RAINBOW_WIN
#include <SDL_timer.h>

namespace Rainbow
{
	/// SDL-implementation of 'get_time' operator.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	struct GetTimeSDL
	{
		unsigned long operator()() const
		{
			return SDL_GetTicks();
		}
	};
}

typedef Rainbow::ChronoBase<Rainbow::GetTimeSDL> Chrono;

#endif
