// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Platform.h"

#include <lua.hpp>

#include "Platform/SystemInfo.h"

NS_RAINBOW_LUA_MODULE_BEGIN(Platform)
{
	void init(lua_State *L)
	{
		lua_pushliteral(L, "platform");
		lua_createtable(L, 0, 4);

		luaR_rawsetfield(
		    L, lua_pushboolean, SystemInfo::has_accelerometer(),
		    "accelerometer");

		// Retrieve locale
		lua_pushliteral(L, "locale");
		{
			Vector<std::unique_ptr<char[]>> locales;
			SystemInfo::locales(locales);
			lua_createtable(L, locales.size(), 0);
			for (size_t i = 0; i < locales.size(); ++i)
			{
				lua_pushstring(L, locales[i].get());
				lua_rawseti(L, -2, i + 1);
			}
		}
		lua_rawset(L, -3);

		// Retrieve physical memory
		lua_pushliteral(L, "memory");
		lua_pushinteger(L, SystemInfo::memory());
		lua_rawset(L, -3);

		// rainbow.platform.screen
		lua_pushliteral(L, "screen");
		lua_createtable(L, 0, 3);
		luaR_rawsetfield(
		    L, lua_pushboolean, SystemInfo::has_touchscreen(), "touch");
		lua_rawset(L, -3);

		lua_rawset(L, -3);
	}

	void update(lua_State *L, const Vec2i &screen)
	{
		lua_getglobal(L, "rainbow");
		lua_pushliteral(L, "platform");
		lua_rawget(L, -2);
		lua_pushliteral(L, "screen");
		lua_rawget(L, -2);

		luaR_rawsetfield(L, lua_pushnumber, screen.width, "width");
		luaR_rawsetfield(L, lua_pushnumber, screen.height, "height");

		lua_pop(L, 3);
	}
} NS_RAINBOW_LUA_MODULE_END(Platform)
