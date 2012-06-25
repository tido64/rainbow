// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include "Common/ShutdownSequence.h"

extern bool active;
extern bool done;

void ShutdownSequence::operator()()
{
	active = false;
	done = true;
}

#endif
