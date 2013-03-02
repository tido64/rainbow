// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform/Definitions.h"
#ifndef RAINBOW_IOS

#include "Common/ShutdownSequence.h"

extern bool active;
extern bool done;

void ShutdownSequence::operator()()
{
	active = false;
	done = true;
}

#endif
