#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

#include "../Hardware/Screen.h"
#include "../Lua.h"

class lua_Platform
{
public:
	lua_Platform(lua_State *, const int);

	// rainbow.platform.screen
	static int height(lua_State *);
	static int touch(lua_State *);
	static int width(lua_State *);
};

#endif
