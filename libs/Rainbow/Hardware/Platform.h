/// Platform specific definitions. Makes sure correct code is being compiled.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(__ANDROID__)
#	define ONWIRE_ANDROID
#elif defined(__APPLE__)
#	define ONWIRE_IOS
#endif

#endif
