/// Platform-specific macros. Makes sure correct code is being compiled.
///
/// \see http://sourceforge.net/p/predef/wiki/Home/
///
/// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
/// Distributed under the MIT License.
/// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_MACROS_H_
#define PLATFORM_MACROS_H_

#include <cstddef>

#if defined(__ANDROID__)
#   define RAINBOW_OS_ANDROID
#elif defined(__APPLE__)
#   if !defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#       define RAINBOW_OS_MACOS
#       define RAINBOW_OS_UNIX
#   else
#       define RAINBOW_OS_IOS
#   endif
#elif defined(__linux__)
#   define RAINBOW_OS_LINUX
#   define RAINBOW_OS_UNIX
#elif defined(_WIN32)
#   define RAINBOW_OS_WINDOWS
#elif defined(__EMSCRIPTEN__)
#   define RAINBOW_JS
#   define RAINBOW_OS_UNIX
#endif

// Platforms that make use of physical buttons (keyboards, gamepads, etc.)
// and of SDL.
#if defined(RAINBOW_OS_UNIX) || defined(RAINBOW_OS_WINDOWS)
#   define RAINBOW_SDL
#endif

#define RAINBOW_BUILD \
    "Rainbow / Bifrost Entertainment Property / Built " __DATE__

#if defined(__GNUC__)
#   if defined(__clang__)
#       define ASSUME(expr) __builtin_assume(expr)
#   else
#       define ASSUME(expr) static_cast<void>(0)
#   endif
#   define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#   define ASSUME(expr) __assume(expr)
#   define UNREACHABLE() __assume(0)
#endif

// TODO: Workaround for missing C++14 features in Android NDK.
#ifdef RAINBOW_OS_ANDROID
#include <cmath>  // std::round

namespace std
{
    template <typename T, size_t N>
    auto cbegin(T (&container)[N]) { return container; }

    template <typename T, size_t N>
    auto cend(T (&container)[N]) { return &container[N]; }

    template <typename T>
    auto cbegin(T& container) { return container.cbegin(); }

    template <typename T>
    auto cend(T& container) { return container.cend(); }

    template <typename T>
    auto rbegin(T& container) { return container.rbegin(); }

    template <typename T>
    auto rend(T& container) { return container.rend(); }

    using ::round;
}
#endif  // RAINBOW_OS_ANDROID

#endif  // PLATFORM_MACROS_H_
