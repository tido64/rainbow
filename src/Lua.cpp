#include "Lua.h"

Lua::Lua() : L(luaL_newstate())
{
	const char rainbow[] = "rainbow";
	luaL_openlibs(this->L);

	// Initialize "rainbow" namespace
	lua_createtable(this->L, 0, 0);
	lua_pushvalue(this->L, -1);
	lua_setfield(this->L, LUA_GLOBALSINDEX, rainbow);

	this->input.init(this->L);         // Initialize "rainbow.input" namespace
	this->platform.init(this->L);      // Initialize "rainbow.platform" namespace

	lua_Algorithm algorithm(this->L);  // Initialize "rainbow.algorithm" namespace
	lua_Audio audio(this->L);          // Initialize "rainbow.audio" namespace
	lua_Physics physics(this->L);      // Initialize "rainbow.physics" namespace

	lua_pop(this->L, 1);

	Lua::wrap<lua_Font>(rainbow);
	Lua::wrap<lua_Sprite>(rainbow);
	Lua::wrap<lua_SpriteBatch>(rainbow);
	Lua::wrap<lua_Texture>(rainbow);

	// We need to set LUA_PATH
	#ifdef RAINBOW_IOS
		AssetManager::Instance().set_source();
	#endif
	const char *bundle = AssetManager::Instance().get_full_path();

	lua_getfield(this->L, LUA_GLOBALSINDEX, "package");
	lua_getfield(this->L, -1, "path");

	size_t path_len = 0;
	const char *pkg_path = lua_tolstring(this->L, -1, &path_len);
	char *lua_path = new char[strlen(bundle) + 8 + path_len];
	strcpy(lua_path, bundle);
	strcat(lua_path, "/?.lua;");
	strcat(lua_path, pkg_path);

	lua_pushstring(this->L, lua_path);
	lua_setfield(this->L, -3, "path");
	lua_pop(this->L, 2);

	delete[] lua_path;
}

void Lua::call(const char *const k)
{
	lua_getfield(this->L, LUA_GLOBALSINDEX, k);
	int lua_e = lua_pcall(this->L, 0, 0, 0);
	if (lua_e)
		this->err(lua_e);
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
	assert(!"Lua related error, see stdout");
}

void Lua::load(const char *const lua)
{
	// Load Lua script
	int lua_e = luaL_loadfile(this->L, lua);
	if (lua_e)
		this->err(lua_e);

	lua_e = lua_pcall(this->L, 0, LUA_MULTRET, 0);
	if (lua_e)
		this->err(lua_e);
}

void Lua::update()
{
#ifdef RAINBOW_ACCELERATED
	this->input.accelerate(this->L);
#endif

	this->platform.update(this->L);
	this->call("update");
}
