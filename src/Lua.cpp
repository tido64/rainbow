#include "Lua.h"

Lua::Lua() : L(luaL_newstate())
{
	const char *rainbow = "rainbow";
	luaL_openlibs(this->L);

	// Create "rainbow" namespace
	lua_createtable(this->L, 0, 0);
	lua_pushvalue(this->L, -1);
	lua_setfield(this->L, LUA_GLOBALSINDEX, rainbow);

	// Create "rainbow.algorithm" namespace
	lua_Algorithm algorithm(this->L);

	// Create "rainbow.audio" namespace
	lua_Audio audio(this->L);

	// Create "rainbow.input" namespace
	this->input.init(this->L);

	// Create "rainbow.platform" namespace
	this->platform.init(this->L);

	lua_pop(this->L, 1);

	Lua::wrap<lua_Font>(rainbow);
	Lua::wrap<lua_Physics>(rainbow);
	Lua::wrap<lua_Sprite>(rainbow);
	Lua::wrap<lua_SpriteBatch>(rainbow);
	Lua::wrap<lua_Texture>(rainbow);

	// On some platforms, we need to set LUA_PATH

#ifdef RAINBOW_IOS

	AssetManager::Instance().set_source();
	const char *bundle = AssetManager::Instance().get_full_path();

	lua_getfield(this->L, LUA_GLOBALSINDEX, "package");
	lua_getfield(this->L, -1, "path");

	size_t path_len;
	const char *pkg_path = lua_tolstring(this->L, -1, &path_len);
	char *lua_path = new char[path_len + strlen(bundle) + 8];
	strcpy(lua_path, pkg_path);
	strcat(lua_path, ";");
	strcat(lua_path, bundle);
	strcat(lua_path, "/?.lua");

	lua_pushstring(this->L, lua_path);
	lua_setfield(this->L, -3, "path");
	lua_pop(this->L, 2);

	delete[] lua_path;

#endif
}

void Lua::call(const char *k)
{
	lua_getfield(this->L, LUA_GLOBALSINDEX, k);
	int lua_e = lua_pcall(this->L, 0, 0, 0);
	if (lua_e != 0) this->err(lua_e);
}

void Lua::err(const int lua_e)
{
	const char *m = lua_tolstring(this->L, -1, 0);
	lua_pop(this->L, 1);
	switch (lua_e)
	{
		case LUA_ERRRUN:
			printf("Lua runtime");
			break;
		case LUA_ERRSYNTAX:
			printf("Lua syntax");
			break;
		case LUA_ERRMEM:
			printf("Lua memory allocation");
			break;
		case LUA_ERRERR:
			printf("Lua error handling");
			break;
		default:
			printf("Unknown Lua");
			break;
	}
	printf(" error: %s\n", m);
	Lua::dump_stack(this->L);
	exit(1);
}

void Lua::load(const char *lua)
{
	// Load Lua script
	int lua_e = luaL_loadfile(this->L, lua);
	if (lua_e != 0) this->err(lua_e);

	lua_e = lua_pcall(this->L, 0, LUA_MULTRET, 0);
	if (lua_e != 0) this->err(lua_e);
}

void Lua::update()
{
	this->input.update(this->L);
	this->platform.update(this->L);
	this->call("update");
}
