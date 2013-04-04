/// Platform specific definitions. Makes sure correct code is being compiled.
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef PLATFORM_DEFINITIONS_H_
#define PLATFORM_DEFINITIONS_H_

#include <cstddef>

#if defined(__ANDROID__)
#	define RAINBOW_ANDROID
#elif defined(__APPLE__)
#	if !defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#		define RAINBOW_MAC
#		define RAINBOW_UNIX
#	else
#		define RAINBOW_IOS
#	endif
#elif defined(__linux__)
#	define RAINBOW_UNIX
#elif defined(_WIN32)
#	define RAINBOW_WIN
#	if defined(_MSC_VER) && (defined(_M_X64) || _M_IX86_FP >= 2)
#		define __SSE2__ 1
#	endif
#elif defined(EMSCRIPTEN)
#	define RAINBOW_JS
#endif

// Platforms that make use of physical buttons (keyboards, gamepads, etc.)
// and of SDL.
#if defined(RAINBOW_JS) || defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)
#	define RAINBOW_BUTTONS
#	define RAINBOW_SDL
#endif

#define RAINBOW_BUILD "Rainbow / Bifrost Entertainment Property / Built " __DATE__

// Define keyword 'override' for pre-C++11 compilers.
#ifdef __GNUC__
#	ifndef __GNUC_PATCHLEVEL__
#		define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#	else
#		define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	endif
#	if __GNUC_VERSION__ < 40700
#		define override
#	endif
#endif

#endif  // PLATFORM_DEFINITIONS_H_
