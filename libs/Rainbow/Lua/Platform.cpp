#include "Platform.h"

lua_Platform::lua_Platform(lua_State *L, const int ns)
{
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, ns, "platform");

	// rainbow.platform.screen
	{
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "screen");

		lua_pushcclosure(L, &lua_Platform::height, 0);
		lua_setfield(L, -2, "height");

		lua_pushcclosure(L, &lua_Platform::touch, 0);
		lua_setfield(L, -2, "touch");

		lua_pushcclosure(L, &lua_Platform::width, 0);
		lua_setfield(L, -2, "width");

		lua_pop(L, 1);
	}

	lua_pop(L, 1);
}

int lua_Platform::height(lua_State *L)
{
	lua_pushnumber(L, Screen::Instance().height());
	return 1;
}

int lua_Platform::touch(lua_State *L)
{
#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
	lua_pushboolean(L, 1);
#else
	lua_pushboolean(L, 0);
#endif

	return 1;
}

int lua_Platform::width(lua_State *L)
{
	lua_pushnumber(L, Screen::Instance().width());
	return 1;
}
