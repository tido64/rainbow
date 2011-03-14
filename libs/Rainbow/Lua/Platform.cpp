#include "Platform.h"

lua_Platform::lua_Platform(lua_State *L, const int ns)
{
	lua_createtable(L, 0, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, ns, "platform");

	// rainbow.platform.screen
	lua_pushcclosure(L, &lua_Platform::screen, 0);
	lua_setfield(L, -2, "screen");

	lua_pop(L, 1);
}

int lua_Platform::screen(lua_State *L)
{
	lua_createtable(L, 0, 0);

	lua_pushnumber(L, Screen::Instance().width());
	lua_setfield(L, -2, "width");

	lua_pushnumber(L, Screen::Instance().height());
	lua_setfield(L, -2, "height");

#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
	lua_pushboolean(L, 1);
#else
	lua_pushboolean(L, 0);
#endif
	lua_setfield(L, -2, "touch");

	return 1;
}
