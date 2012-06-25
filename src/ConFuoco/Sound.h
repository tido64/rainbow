#ifndef CONFUOCO_SOUND_H_
#define CONFUOCO_SOUND_H_

#include "Platform.h"
#if defined(RAINBOW_ANDROID)
#	include "ConFuoco/impl/Sound_SL.h"
#elif defined(RAINBOW_IOS) || defined(RAINBOW_SDL)
#	include "ConFuoco/impl/Sound_AL.h"
#else
#	error "Unknown platform"
#endif

#endif
