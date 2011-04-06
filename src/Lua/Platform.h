#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

#include "../Hardware/Screen.h"

class lua_Platform
{
public:
	void init(lua_State *L)
	{
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "platform");

	#ifdef RAINBOW_ACCELERATED
		lua_pushboolean(L, 1);
	#else
		lua_pushboolean(L, 0);
	#endif
		lua_setfield(L, -2, "accelerometer");

		// rainbow.platform.screen
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "screen");

	#ifdef RAINBOW_TOUCHED
		lua_pushboolean(L, 1);
	#else
		lua_pushboolean(L, 0);
	#endif
		lua_setfield(L, -2, "touch");

		lua_pop(L, 2);
		this->update(L);
	}

	/// Updates rainbow.platform.screen
	void update(lua_State *L)
	{
		lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
		lua_getfield(L, -1, "platform");
		lua_getfield(L, -1, "screen");

		lua_pushnumber(L, Screen::Instance().width());
		lua_setfield(L, -2, "width");

		lua_pushnumber(L, Screen::Instance().height());
		lua_setfield(L, -2, "height");

		lua_pop(L, 2);
	}
};

#endif
