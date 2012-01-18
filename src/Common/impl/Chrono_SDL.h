#ifdef RAINBOW_SDL
#include <SDL/SDL_timer.h>

/// SDL-implementation of Chrono.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Chrono : public Rainbow::_Chrono<Chrono>
{
public:
	inline Chrono();
	inline unsigned long get_time();

private:
	/// Intentionally left undefined.
	Chrono(const Chrono &);

	/// Intentionally left undefined.
	Chrono& operator=(const Chrono &);
};

Chrono::Chrono() { }

unsigned long Chrono::get_time()
{
	return SDL_GetTicks();
}

#endif
