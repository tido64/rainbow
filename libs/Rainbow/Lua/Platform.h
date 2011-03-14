#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

#include "../Hardware/Screen.h"
#include "../Lua.h"

class lua_Platform
{
public:
	lua_Platform(lua_State *, const int);

	/// rainbow.platform.screen returns width, height and touch capabilities
	static int screen(lua_State *);
};

#endif
