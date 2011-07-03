/// Includes correct OpenAL headers for corresponding platforms.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef OPENAL_H_
#define OPENAL_H_

#include "Platform.h"

#ifdef RAINBOW_IOS
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

#endif
