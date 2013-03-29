// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Config.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	Config::Config() : accelerometer(true), width(0), height(0)
	{
		Data config("config");
		if (!config)
			return;

		lua_State *L = luaL_newstate();
		Rainbow::Lua::load(L, config, "config");
		lua_getglobal(L, "accelerometer");
		this->accelerometer = lua_toboolean(L, -1);
		lua_getglobal(L, "resolution");
		if (lua_istable(L, -1))
		{
			lua_rawgeti(L, -1, 1);
			this->width = luaR_tointeger(L, -1);
			lua_rawgeti(L, -2, 2);
			this->height = luaR_tointeger(L, -1);
		}
		lua_close(L);
	}
}
