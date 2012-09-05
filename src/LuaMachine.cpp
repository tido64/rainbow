// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Lua/Modules.h"

void LuaMachine::dump_stack(lua_State *L)
{
	printf("Lua stack size: %i\n", lua_gettop(L));
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
				printf("%s\n", lua_tolstring(L, l, nullptr));
				break;
			case LUA_TTABLE:
				puts("(table)");
				break;
			case LUA_TFUNCTION:
				printf("%p\n", lua_topointer(L, l));
				break;
			case LUA_TUSERDATA:
				printf("%p\n", lua_touserdata(L, l));
				break;
			case LUA_TTHREAD:
				puts("(thread)");
				break;
			case LUA_TLIGHTUSERDATA:
				printf("%p\n", lua_topointer(L, l));
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

int LuaMachine::load(lua_State *L, const Data &chunk, const char *name, const bool load)
{
	int e = luaL_loadbuffer(L, chunk, chunk.size(), name);
	if (e || (load && (e = lua_pcall(L, 0, LUA_MULTRET, 0))))
	{
		err(L, e);
		return luaL_error(L, "Failed to load '%s'", name);
	}
	return 1;
}

LuaMachine::~LuaMachine()
{
	delete this->scenegraph;
	lua_close(this->L);
}

int LuaMachine::update(const unsigned long t)
{
	lua_pushinteger(this->L, t);
	return this->call("update", 1);
}

void LuaMachine::err(lua_State *L, const int lua_e)
{
	R_ASSERT(lua_e != LUA_OK, "No error to report");

	const char *err_general = "general";
	const char *err_runtime = "runtime";
	const char *err_syntax  = "syntax";
	const char *err_memory  = "memory allocation";
	const char *err_errfunc = "error handling";

	const char *desc = err_general;
	switch (lua_e)
	{
		case LUA_ERRRUN:
			desc = err_runtime;
			break;
		case LUA_ERRSYNTAX:
			desc = err_syntax;
			break;
		case LUA_ERRMEM:
			desc = err_memory;
			break;
		case LUA_ERRERR:
			desc = err_errfunc;
			break;
		default:
			break;
	}
	R_ERROR("Lua %s error: %s\n", desc, lua_tolstring(L, -1, nullptr));
	lua_pop(L, 1);
	dump_stack(L);
}

int LuaMachine::load(lua_State *L)
{
	const char *module = lua_tostring(L, -1);
	char *filename = new char[strlen(module) + 5];
	strcpy(filename, module);
	strcat(filename, ".lua");

	Data file(filename);
	delete[] filename;
	return (!file) ? 0 : LuaMachine::load(L, file, module, false);
}

LuaMachine::LuaMachine(SceneGraph::Node *root) : scenegraph(nullptr), L(luaL_newstate())
{
	luaL_openlibs(this->L);

	// Initialize "rainbow" namespace
	lua_createtable(this->L, 0, 16);
	lua_pushvalue(this->L, -1);
	lua_setglobal(this->L, "rainbow");
	Rainbow::Lua::init(this->L);

	// Initialize "rainbow.scenegraph"
	this->scenegraph = new Rainbow::Lua::SceneGraph(this->L, root);

	// Bind C++ objects
	Rainbow::Lua::bind(this->L);

	// Clean up the stack
	lua_pop(this->L, 1);
	R_ASSERT(lua_gettop(L) == 0, "Stack not empty");

	// Set up custom loader
	lua_getglobal(this->L, "package");
	R_ASSERT(!lua_isnil(this->L, -1), "package table does not exist");
	lua_getfield(this->L, -1, "searchers");
	R_ASSERT(!lua_isnil(this->L, -1), "package.searchers table does not exist");

	// Make space in the second slot for our loader
	for (size_t i = lua_rawlen(L, -1); i > 1; --i)
	{
		lua_rawgeti(this->L, -1, i);
		lua_rawseti(this->L, -2, i + 1);
	}
	lua_pushcfunction(this->L, load);
	lua_rawseti(this->L, -2, 1);

	// Clean up the stack
	lua_pop(this->L, 2);
	R_ASSERT(lua_gettop(L) == 0, "Stack not empty");
}

int LuaMachine::call(const char *const k, int nargs, int nresults)
{
	R_ASSERT(lua_gettop(L) == nargs, "Number of arguments != stack size");

	lua_getglobal(this->L, k);
	lua_insert(this->L, 1);

#ifndef NDEBUG
	lua_getglobal(this->L, "debug");
	lua_getfield(this->L, -1, "traceback");
	lua_insert(this->L, 1);
	lua_pop(this->L, 1);
	const int lua_e = lua_pcall(this->L, nargs, nresults, 1);
	lua_remove(this->L, 1);
#else
	const int lua_e = lua_pcall(this->L, nargs, nresults, 0);
#endif

	if (lua_e != LUA_OK)
	{
		err(this->L, lua_e);
		return luaL_error(L, "Failed to call '%s'", k);
	}
	return LUA_OK;
}
