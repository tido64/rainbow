#include "Random.h"

#ifdef RAINBOW_UNIX
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif

#ifndef RAINBOW_IOS
#	include <dSFMT/dSFMT.c>
#endif

#ifdef RAINBOW_UNIX
#	pragma GCC diagnostic pop
#endif
