// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include "Common/Random.h"

#ifndef RAINBOW_IOS
#	ifdef RAINBOW_UNIX
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wold-style-cast"
#	endif
#	include <dSFMT/dSFMT.c>
#	ifdef RAINBOW_UNIX
#		pragma GCC diagnostic pop
#	endif
#endif
