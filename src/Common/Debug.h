#ifndef RAINBOW_DEBUG_H_
#define RAINBOW_DEBUG_H_

#include <cstdio>

#include "Platform.h"
#ifdef RAINBOW_ANDROID
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

#define LUA_ASSERT(expr, syntax) \
	do \
	{ \
		if (!(expr)) \
		{ \
			R_ERROR("[Rainbow] Lua syntax: %s\n", syntax); \
			unsigned int depth = 0; \
			lua_Debug entry; \
			while (lua_getstack(L, depth, &entry)) \
			{ \
				lua_getinfo(L, "nSl", &entry); \
				R_ERROR("%s:%d: %s\n", entry.short_src, entry.currentline, entry.name ? entry.name : ""); \
				++depth; \
			} \
			abort(); \
		} \
	} while (0)

#define LUA_CHECK(L, expr, reason) \
	if (!(expr)) \
		luaL_error(L, reason)

#define R_ASSERT(expr, reason) \
	do \
	{ \
		if (!(expr)) \
		{ \
			R_ERROR("[Rainbow] %s: %s (aborted at %s:%i: %s)\n", __PRETTY_FUNCTION__, reason, __FILE__, __LINE__, #expr); \
			abort(); \
		} \
	} while (0)

#ifdef RAINBOW_ANDROID
#	define R_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "Rainbow", __VA_ARGS__)
#else
#	define R_DEBUG(...) do { fprintf(stdout, __VA_ARGS__); fflush(stdout); } while (0)
#endif

#ifndef __GXX_EXPERIMENTAL_CXX0X__

namespace Rainbow
{
	template<bool>
	struct _static_assert;

	/// Static assert is inspired by Boost.StaticAssert and aims to implement
	/// simple compile-time assertion.
	///
	/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	template<>
	struct _static_assert<true> { };
}

#define static_assert(expr, reason) Rainbow::_static_assert<(expr)>()
#endif

#else

#define LUA_ASSERT(expr, syntax)   static_cast<void>(0)
#define LUA_CHECK(L, expr, reason) static_cast<void>(0)
#define R_ASSERT(expr, reason)     static_cast<void>(0)
#define R_DEBUG(...)               static_cast<void>(0)

#ifndef __GXX_EXPERIMENTAL_CXX0X__
#	define static_assert(expr, reason) static_cast<void>(0)
#endif

#endif  // !NDEBUG
#endif  // RAINBOW_DEBUG_H_
