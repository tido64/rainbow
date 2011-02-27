#include "Lua.h"

Lua::Lua() : L(lua_open())
{
	luaL_openlibs(this->L);

	// Create "rainbow" namespace
	lua_newtable(this->L);
	const int rainbow = lua_gettop(this->L);
	lua_pushvalue(this->L, -1);
	lua_setfield(this->L, LUA_GLOBALSINDEX, "rainbow");

	// Create "rainbow.algorithm" namespace
	lua_Algorithm algorithm(this->L, rainbow);

	// Create "rainbow.platform" namespace
	lua_Platform platform(this->L, rainbow);

	lua_pop(this->L, 1);

	Lua::wrap<lua_Font>("rainbow");
	Lua::wrap<lua_Physics>("rainbow");
	Lua::wrap<lua_Sprite>("rainbow");
	Lua::wrap<lua_Texture>("rainbow");
}

void Lua::call(const char *k)
{
	lua_getfield(this->L, LUA_GLOBALSINDEX, k);
	lua_pcall(this->L, 0, 0, 0);
}

void Lua::load(const char *lua)
{
	// Load Lua script
	int lua_e = luaL_loadfile(this->L, lua);
	if (lua_e != 0)
	{
		switch (lua_e)
		{
			case LUA_ERRSYNTAX:
				printf("Syntax error: %s\n", lua);
				break;
			case LUA_ERRMEM:
				printf("Memory allocation error: %s\n", lua);
				break;
			default:
				printf("Read error: %s\n", lua);
				break;
		}
		exit(1);
	}
	lua_e = lua_pcall(this->L, 0, LUA_MULTRET, 0);
	assert(lua_e == 0);

	// Run initialiser
	this->call("init");
}
