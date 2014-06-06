// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUASYNTAX_H_
#define LUA_LUASYNTAX_H_

#include "Lua/LuaMacros.h"

struct lua_State;

NS_RAINBOW_LUA_BEGIN
{
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
