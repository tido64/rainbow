// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_

#include <cstdio>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID
#	include <android/log.h>
#	define R_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "Rainbow", __VA_ARGS__)
#else
#	define R_ERROR(...) fprintf(stderr, __VA_ARGS__)
#endif

#ifndef NDEBUG

#ifndef __GNUC__
#	define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#include <cstdlib>

#define LUA_CHECK(L, expr, ...) \
	if (!(expr)) \
		luaL_error(L, __VA_ARGS__)

#define R_ASSERT(expr, reason) \
	do \
	{ \
		if (!(expr)) \
		{ \
			R_ERROR("[Rainbow] %s: %s (aborted at %s:%i: %s)\n", __PRETTY_FUNCTION__, reason, __FILE__, __LINE__, #expr); \
			abort(); \
		} \
	} while (0)

#ifdef RAINBOW_OS_ANDROID
#	define R_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "Rainbow", __VA_ARGS__)
#else
#	define R_DEBUG(...) do { fprintf(stdout, __VA_ARGS__); fflush(stdout); } while (0)
#endif

#else

#define LUA_CHECK(L, expr, ...)   static_cast<void>(0)
#define R_ASSERT(expr, reason)    static_cast<void>(0)
#define R_DEBUG(...)              static_cast<void>(0)

#endif  // !NDEBUG
#endif  // RAINBOW_DEBUG_H_
