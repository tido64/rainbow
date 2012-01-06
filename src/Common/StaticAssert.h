#ifndef STATICASSERT_H_
#define STATICASSERT_H_

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

#ifdef NDEBUG
#	define static_assert(expr) static_cast<void>(0)
#else
#	define static_assert(expr) Rainbow::_static_assert<(expr)>()
#endif

#endif
