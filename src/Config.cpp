// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Config.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	Config::Config()
	    : accelerometer_(true), suspend_(true), width_(0), height_(0)
	{
		const char kConfigModule[] = "config";

		const Path path(kConfigModule, Path::RelativeTo::CurrentPath);
		if (!path.is_file())
		{
			R_DEBUG("[Rainbow] No config file was found\n");
			return;
		}

		const Data config(File::open(path));
		if (!config)
			return;

		lua_State *L = luaL_newstate();
		if (Rainbow::Lua::load(L, config, kConfigModule) == 0)
			return;

		lua_getglobal(L, "accelerometer");
		if (lua_isboolean(L, -1))
			accelerometer_ = lua_toboolean(L, -1);

		lua_getglobal(L, "resolution");
		if (lua_istable(L, -1))
		{
			lua_rawgeti(L, -1, 1);
			width_ = luaR_tointeger(L, -1);
			lua_rawgeti(L, -2, 2);
			height_ = luaR_tointeger(L, -1);
		}

	#ifdef RAINBOW_SDL
		lua_getglobal(L, "suspend_on_focus_lost");
		if (lua_isboolean(L, -1))
			suspend_ = lua_toboolean(L, -1);
	#endif

		lua_close(L);
	}
}
