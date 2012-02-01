// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"

#ifdef RAINBOW_SDL
#include "Common/ShutdownSequence.h"

extern bool done;

void ShutdownSequence::operator()()
{
	done = true;
}

#endif
