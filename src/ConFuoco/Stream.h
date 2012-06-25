#ifndef CONFUOCO_STREAM_H_
#define CONFUOCO_STREAM_H_

#include "Platform.h"
#if defined(RAINBOW_ANDROID)
#	include "ConFuoco/impl/Stream_SL.h"
#elif defined(RAINBOW_IOS)
#	include "ConFuoco/impl/Stream_iOS.h"
#elif defined(RAINBOW_SDL)
#	include "ConFuoco/impl/Stream_AL.h"
#else
#	error "Unknown platform"
#endif

#endif
