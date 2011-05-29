/// Platform specific definitions. Makes sure correct code is being compiled.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(__ANDROID__)
#	define RAINBOW_ANDROID
#	define RAINBOW_ZIP
#elif defined(__APPLE__)
#	define RAINBOW_IOS
#elif defined(__linux)
#	define RAINBOW_UNIX
#elif defined(_WIN32)
#	define RAINBOW_WIN
#endif

// Platforms that make use of buttons (keyboards, gamepads, etc.)
#if defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)
#	define RAINBOW_BUTTONS
#endif

// Platforms with accelerometer and touch screen
#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
#	define RAINBOW_ACCELERATED
#	define RAINBOW_TOUCHED
#endif

#define RAINBOW_BUILD "Rainbow / Bifrost Games Property / Built " __DATE__

#endif
