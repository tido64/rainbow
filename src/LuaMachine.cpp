#include "LuaMachine.h"

#if __WORDSIZE == 32
#	define PRINT_ADDRESS(addr) printf("%x\n", static_cast<unsigned int>(reinterpret_cast<uintptr_t>(addr)))
#elif __WORDSIZE == 64
#	define PRINT_ADDRESS(addr) printf("%lx\n", static_cast<unsigned int>(reinterpret_cast<uintptr_t>(addr)))
#else
#	error "Unknown system"
#endif

void LuaMachine::dump_stack(lua_State *L)
{
	puts("Lua stack");
	for (int l = 1; l <= lua_gettop(L); ++l)
	{
		printf("#%i: ", l);
		switch (lua_type(L, l))
		{
			case LUA_TNIL:
				puts("(nil)");
				break;
			case LUA_TNUMBER:
				printf(LUA_NUMBER_FMT "\n", lua_tonumber(L, l));
				break;
			case LUA_TBOOLEAN:
				printf("%s\n", lua_toboolean(L, l) ? "true" : "false");
				break;
			case LUA_TSTRING:
				printf("%s\n", lua_tolstring(L, l, 0));
				break;
			case LUA_TTABLE:
				puts("(table)");
				break;
			case LUA_TFUNCTION:
				PRINT_ADDRESS(lua_tocfunction(L, l));
				break;
			case LUA_TUSERDATA:
				PRINT_ADDRESS(lua_touserdata(L, l));
				break;
			case LUA_TTHREAD:
				puts("(thread)");
				break;
			case LUA_TLIGHTUSERDATA:
				PRINT_ADDRESS(lua_topointer(L, l));
				break;
			default:
				puts("(unknown)");
				break;
		}
	}
}

lua_Debug* LuaMachine::getinfo(lua_State *L)
{
	lua_Debug *d = new lua_Debug();
	lua_getstack(L, 1, d);
	lua_getinfo(L, "nSl", d);
	return d;
}

LuaMachine::LuaMachine() : L(luaL_newstate())
{
	const char rainbow[] = "rainbow";
	luaL_openlibs(this->L);

	// Initialize "rainbow" namespace
	lua_createtable(this->L, 0, 16);
	lua_pushvalue(this->L, -1);
	lua_setfield(this->L, LUA_GLOBALSINDEX, rainbow);

	lua_Platform::init(this->L);   // Initialize "rainbow.platform" namespace
	lua_Input::init(this->L);      // Initialize "rainbow.input" namespace
	lua_Algorithm::init(this->L);  // Initialize "rainbow.algorithm" namespace
	lua_Audio::init(this->L);      // Initialize "rainbow.audio" namespace
	lua_Physics::init(this->L);    // Initialize "rainbow.physics" namespace

	lua_pop(this->L, 1);

	LuaMachine::wrap<lua_Font>(rainbow);
	LuaMachine::wrap<lua_Sprite>(rainbow);
	LuaMachine::wrap<lua_SpriteBatch>(rainbow);
	LuaMachine::wrap<lua_Texture>(rainbow);

	// We need to set LUA_PATH
	const char *const bundle = Data::get_path();

	lua_getfield(this->L, LUA_GLOBALSINDEX, "package");
	lua_getfield(this->L, -1, "path");

	size_t path_len = 0;
	const char *const pkg_path = lua_tolstring(this->L, -1, &path_len);
	char *const lua_path = new char[strlen(bundle) + 8 + path_len];
	strcpy(lua_path, bundle);
	strcat(lua_path, "/?.lua;");
	strcat(lua_path, pkg_path);

	lua_pushstring(this->L, lua_path);
	lua_setfield(this->L, -3, "path");
	lua_pop(this->L, 2);

	Data::free(lua_path);
	assert(lua_gettop(L) == 0 || !"LuaMachine::LuaMachine: Stack not empty");
}

void LuaMachine::call(const char *const k)
{
	lua_getfield(this->L, LUA_GLOBALSINDEX, k);
	int lua_e = lua_pcall(this->L, 0, 0, 0);
	if (lua_e)
		this->err(lua_e);
}

void LuaMachine::err(const int lua_e)
{
	const char *const m = lua_tolstring(this->L, -1, 0);
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
			printf("Lua");
			break;
	}
	printf(" error: %s\n", m);
	dump_stack(this->L);
	assert(!"Lua related error, see stdout");
}

void LuaMachine::load(const char *const lua)
{
	// Load Lua script
	int lua_e = luaL_loadfile(this->L, lua);
	if (lua_e)
		this->err(lua_e);

	lua_e = lua_pcall(this->L, 0, LUA_MULTRET, 0);
	if (lua_e)
		this->err(lua_e);
}

void LuaMachine::update()
{
#if RAINBOW_ACCELERATED
	lua_Input::accelerate(this->L);
#endif

	this->call("update");
}
