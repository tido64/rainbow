// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Config.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	Config::Config() : accelerometer_(true), width_(0), height_(0)
	{
		Data config("config");
		if (!config)
			return;

		lua_State *L = luaL_newstate();
		if (Rainbow::Lua::load(L, config, "config") == 0)
			return;
		lua_getglobal(L, "accelerometer");
		this->accelerometer_ = lua_toboolean(L, -1);
		lua_getglobal(L, "resolution");
		if (lua_istable(L, -1))
		{
			lua_rawgeti(L, -1, 1);
			this->width_ = luaR_tointeger(L, -1);
			lua_rawgeti(L, -2, 2);
			this->height_ = luaR_tointeger(L, -1);
		}
		lua_close(L);
	}
}
