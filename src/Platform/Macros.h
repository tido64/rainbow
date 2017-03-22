/// Platform-specific macros. Makes sure correct code is being compiled.
///
/// \see http://sourceforge.net/p/predef/wiki/Home/
///
/// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
/// Distributed under the MIT License.
/// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_MACROS_H_
#define PLATFORM_MACROS_H_

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

#define RAINBOW_BUILD "Rainbow / Built " __DATE__
#ifdef USE_HEIMDALL
#   define RAINBOW_WINDOW_TITLE RAINBOW_BUILD " / Press Ctrl+F11 for dev menu"
#else
#   define RAINBOW_WINDOW_TITLE RAINBOW_BUILD
#endif

#if defined(__GNUC__)
#   if defined(__clang__)
#       define ASSUME(expr) __builtin_assume(expr)
#       define CLANG_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100)
#   else
#       define ASSUME(expr) static_cast<void>(0)
#       define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#   endif
#   define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#   define ASSUME(expr) __assume(expr)
#   define UNREACHABLE() __assume(0)
#endif

#define NOT_USED(v) static_cast<void>(v)

#if (defined(__apple_build_version__) && CLANG_VERSION > 80000) ||             \
    (!defined(__apple_build_version__) && CLANG_VERSION >= 30900) ||           \
    GCC_VERSION >= 50300 ||                                                    \
    defined(_MSC_VER) ||                                                       \
    __cpp_lib_experimental_filesystem >= 201406
#   define USE_STD_FILESYSTEM 1
#else
#   define USE_STD_FILESYSTEM 0
#endif

#endif  // PLATFORM_MACROS_H_
