// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUABIND_H_
#define LUA_LUABIND_H_

#include "Lua/LuaHelper.h"

struct lua_State;

NS_RAINBOW_LUA_BEGIN
{
	template<typename T>
	class Bind
	{
	public:
		static const char class_name[];
		static const bool is_constructible;
		static const luaL_Reg functions[];

	protected:
		static T* self(lua_State *L)
		{
			return touserdata<T>(L, 1);
		}

		~Bind() = default;
	};
} NS_RAINBOW_LUA_END

#endif
