/// Platform-specific macros. Makes sure correct code is being compiled.
///
/// \see http://sourceforge.net/p/predef/wiki/Home/
///
/// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
/// Distributed under the MIT License.
/// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_MACROS_H_
#define PLATFORM_MACROS_H_

#include <cstddef>

#if defined(__ANDROID__)
#	define RAINBOW_OS_ANDROID
#elif defined(__APPLE__)
#	if !defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#		define RAINBOW_OS_MACOS
#		define RAINBOW_OS_UNIX
#	else
#		define RAINBOW_OS_IOS
#	endif
#elif defined(__linux__)
#	define RAINBOW_OS_LINUX
#	define RAINBOW_OS_UNIX
#elif defined(_WIN32)
#	define RAINBOW_OS_WINDOWS
#elif defined(EMSCRIPTEN)
#	define RAINBOW_JS
#	define RAINBOW_OS_UNIX
#endif

// Platforms that make use of physical buttons (keyboards, gamepads, etc.)
// and of SDL.
#if defined(RAINBOW_OS_UNIX) || defined(RAINBOW_OS_WINDOWS)
#	define RAINBOW_BUTTONS
#	define RAINBOW_SDL
#endif

#define RAINBOW_BUILD \
    "Rainbow / Bifrost Entertainment Property / Built " __DATE__

// Workaround for warning C4425:
//     'constexpr' was ignored (class literal types are not yet supported)
#if defined(_MSC_VER) && _MSC_VER <= 1800
#	define constexpr inline
#endif

#endif  // PLATFORM_MACROS_H_
