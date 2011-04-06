/// Platform specific definitions. Makes sure correct code is being compiled.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(__ANDROID__)
#	define RAINBOW_ANDROID
#	define RAINBOW_ACCELERATED
#	define RAINBOW_TOUCHED
#elif defined(__APPLE__)
#	define RAINBOW_IOS
#	define RAINBOW_ACCELERATED
#	define RAINBOW_TOUCHED
#elif defined(__linux)
#	define RAINBOW_NIX
#	define RAINBOW_TACTILE
#elif defined(_WIN32)
#	define RAINBOW_WIN
#	define RAINBOW_TACTILE
#endif

#endif
