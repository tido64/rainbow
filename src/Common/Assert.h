#ifndef RAINBOW_ASSERT_H_
#define RAINBOW_ASSERT_H_

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

#include <cstdio>
#include <cstdlib>

#define LUA_ASSERT(expr, syntax) \
	do \
	{ \
		if (!(expr)) \
		{ \
			fprintf(stderr, "[Rainbow] Lua syntax: %s\n", syntax); \
			abort(); \
		} \
	} while (0)

#define R_ASSERT(expr, reason) \
	do \
	{ \
		if (!(expr)) \
		{ \
			fprintf(stderr, "[Rainbow] Assertion failed in '%s' at line %i: %s (%s)\n", __FILE__, __LINE__, reason, #expr); \
			abort(); \
		} \
	} while (0)

#define static_assert(expr) Rainbow::_static_assert<(expr)>()

#else

#define LUA_ASSERT(expr, syntax) static_cast<void>(0)
#define R_ASSERT(expr, reason)   static_cast<void>(0)
#define static_assert(expr)      static_cast<void>(0)

#endif

#endif
