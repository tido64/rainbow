// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUASYNTAX_H_
#define LUA_LUASYNTAX_H_

#include <utility>

#include "Lua/LuaMacros.h"

#ifndef NDEBUG
#	define LUA_ASSERT(L, expr, ...) \
		Rainbow::Lua::verify(L, (expr), __VA_ARGS__)
#else
#	define LUA_ASSERT(L, expr, ...) static_cast<void>(0)
#endif

extern "C"
{
	struct lua_State;
	extern int (luaL_error) (lua_State *L, const char *fmt, ...);
}

NS_RAINBOW_LUA_BEGIN
{
	template<typename... Args>
	void verify(lua_State *L,
	            const bool success,
	            const char *const error,
	            Args&&... args)
	{
		if (!success)
			luaL_error(L, error, std::forward<Args>(args)...);
	}

	template<typename T>
	struct Argument
	{
		static void is_optional(lua_State *L, const int n);
		static void is_required(lua_State *L, const int n);
	};

#ifdef NDEBUG
	template<typename T>
	void Argument<T>::is_optional(lua_State *, const int) { }

	template<typename T>
	void Argument<T>::is_required(lua_State *, const int) { }
#endif  // NDEBUG
} NS_RAINBOW_LUA_END

#endif  // LUA_LUASYNTAX_H_
