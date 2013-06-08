#if defined(RAINBOW_JS) || defined(RAINBOW_WIN)
#include <SDL_timer.h>

void Chrono::sleep(const unsigned int ms)
{
	SDL_Delay(ms);
}

unsigned long Chrono::time()
{
	return SDL_GetTicks();
}

#endif
