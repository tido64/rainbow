#ifndef RAINBOW_DEBUG_H_
#define RAINBOW_DEBUG_H_

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

#ifndef NDEBUG
#	ifndef __GNUC__
#		define __PRETTY_FUNCTION__ __FUNCTION__
#	endif

#include <cstdio>
#include <cstdlib>

#define LUA_ASSERT(expr, syntax) \
	do \
	{ \
		if (!(expr)) \
		{ \
			fprintf(stderr, "[Rainbow] Lua syntax: %s\n", syntax); \
			unsigned int depth = 0; \
			lua_Debug entry; \
			while (lua_getstack(L, depth, &entry)) \
			{ \
				lua_getinfo(L, "nSl", &entry); \
				fprintf(stderr, "%s:%d: %s\n", entry.short_src, entry.currentline, entry.name ? entry.name : ""); \
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
			fprintf(stderr, "[Rainbow] %s: %s (aborted at %s:%i: %s)\n", __PRETTY_FUNCTION__, reason, __FILE__, __LINE__, #expr); \
			abort(); \
		} \
	} while (0)

#ifndef __GXX_EXPERIMENTAL_CXX0X__
#	define static_assert(expr, reason) Rainbow::_static_assert<(expr)>()
#endif

#else

#define LUA_ASSERT(expr, syntax)   static_cast<void>(0)
#define LUA_CHECK(L, expr, reason) static_cast<void>(0)
#define R_ASSERT(expr, reason)     static_cast<void>(0)

#ifndef __GXX_EXPERIMENTAL_CXX0X__
#	define static_assert(expr, reason) static_cast<void>(0)
#endif

#endif  // !NDEBUG
#endif  // RAINBOW_DEBUG_H_
