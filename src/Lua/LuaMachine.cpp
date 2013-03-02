// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Lua/LuaModules.h"

namespace Rainbow
{
	LuaMachine::~LuaMachine()
	{
		delete this->scenegraph;
		lua_close(this->L);
	}

	int LuaMachine::init(const Data &main)
	{
		Lua::load(this->L, main, "main");

	#ifndef NDEBUG
		lua_rawgeti(this->L, LUA_REGISTRYINDEX, this->traceback);
	#endif
		lua_getglobal(this->L, "init");
	#ifndef NDEBUG
		const int lua_e = lua_pcall(this->L, 0, 0, 1);
		lua_remove(L, 1);
	#else
		const int lua_e = lua_pcall(this->L, 0, 0, 0);
	#endif
		if (lua_e != LUA_OK)
		{
			Lua::error(this->L, lua_e);
			return luaL_error(this->L, "Failed to initialise main script");
		}
		return LUA_OK;
	}

	int LuaMachine::update(const unsigned long t)
	{
	#ifndef NDEBUG
		lua_rawgeti(this->L, LUA_REGISTRYINDEX, this->traceback);
	#endif
		lua_rawgeti(this->L, LUA_REGISTRYINDEX, this->internal);
		lua_pushinteger(this->L, t);
	#ifndef NDEBUG
		const int lua_e = lua_pcall(this->L, 1, 0, 1);
		lua_remove(this->L, 1);
	#else
		const int lua_e = lua_pcall(this->L, 1, 0, 0);
	#endif
		if (lua_e != LUA_OK)
		{
			Lua::error(this->L, lua_e);
			return luaL_error(this->L, "Failed to call 'update'");
		}
		return LUA_OK;
	}

	LuaMachine::LuaMachine(SceneGraph::Node *root) :
		internal(0), traceback(0), scenegraph(nullptr), L(luaL_newstate())
	{
		luaL_openlibs(this->L);

		// Initialize "rainbow" namespace
		lua_createtable(this->L, 0, 16);
		Lua::init(this->L);

		// Initialize "rainbow.scenegraph"
		this->scenegraph = new Lua::SceneGraph(this->L, root);

		// Bind C++ objects
		Lua::bind(this->L);

		lua_setglobal(this->L, "rainbow");
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

		// Set up custom loader
		lua_getglobal(this->L, "package");
		R_ASSERT(!lua_isnil(this->L, -1), "package table does not exist");
		lua_pushliteral(this->L, "searchers");
		lua_rawget(this->L, -2);
		R_ASSERT(!lua_isnil(this->L, -1), "package.searchers table does not exist");

		// Make space in the second slot for our loader
		for (size_t i = lua_rawlen(this->L, -1); i > 1; --i)
		{
			lua_rawgeti(this->L, -1, i);
			lua_rawseti(this->L, -2, i + 1);
		}
		lua_pushcfunction(this->L, Lua::load);
		lua_rawseti(this->L, -2, 1);

		// Clean up the stack
		lua_pop(this->L, 2);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

		// Load our internal script
		const char Rainbow_lua[] =
				"__rainbow_modules = {}\n"
				"function __update(dt)\n"
					"for i = 1, #__rainbow_modules do\n"
						"__rainbow_modules[i](dt)\n"
					"end\n"
					"update(dt)\n"
				"end\n";
		int e = luaL_loadbuffer(this->L, Rainbow_lua, sizeof(Rainbow_lua) / sizeof(char) - 1, "Rainbow");
		R_ASSERT(e == LUA_OK, "Failed to load internal Lua script");
		e = lua_pcall(this->L, 0, LUA_MULTRET, 0);
		R_ASSERT(e == LUA_OK, "Failed to execute internal Lua script");
		lua_getglobal(this->L, "__update");
		R_ASSERT(lua_isfunction(this->L, -1), "Failed to get internal Lua script");
		this->internal = luaL_ref(this->L, LUA_REGISTRYINDEX);

	#ifndef NDEBUG
		lua_getglobal(this->L, "debug");
		lua_pushliteral(this->L, "traceback");
		lua_rawget(this->L, -2);
		this->traceback = luaL_ref(this->L, LUA_REGISTRYINDEX);
		lua_pop(this->L, 1);
	#endif
	}
}
