// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaMachine.h"

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Lua/LuaModules.h"
#include "Lua/lua_Rainbow.h"

namespace
{
	int breakpoint(lua_State *L)
	{
	#ifndef NDEBUG
		Rainbow::Lua::sethook(L);
	#else
		static_cast<void>(L);
	#endif
		return 0;
	}

	int createtable(lua_State *L)
	{
		lua_createtable(L, luaR_optinteger(L, 1, 0), luaR_optinteger(L, 2, 0));
		return 1;
	}

	// Partially copied from 'linit.c'.
	void luaR_openlibs(lua_State *L)
	{
		const luaL_Reg loadedlibs[] = {
			{ "_G", luaopen_base },
			{ LUA_LOADLIBNAME, luaopen_package },
			{ LUA_COLIBNAME, luaopen_coroutine },
			{ LUA_TABLIBNAME, luaopen_table },
			{ LUA_STRLIBNAME, luaopen_string },
			{ LUA_BITLIBNAME, luaopen_bit32 },
			{ LUA_MATHLIBNAME, luaopen_math },
			{ LUA_DBLIBNAME, luaopen_debug },
			{ nullptr, nullptr }
		};
		/* call open functions from 'loadedlibs' and set results to global table */
		for (const luaL_Reg *lib = loadedlibs; lib->func; lib++)
		{
			luaL_requiref(L, lib->name, lib->func, 1);
			lua_pop(L, 1);  /* remove lib */
		}
	}

	int time_since_epoch(lua_State *L)
	{
		lua_pushinteger(L, Chrono::time_since_epoch().count());
		return 1;
	}
}

namespace Rainbow
{
	int LuaMachine::start(const Data &main)
	{
		if (Lua::load(this->L, main, "main") == 0)
			return luaL_error(this->L, "Failed to load main script");

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

	LuaMachine::LuaMachine()
	    : internal(0), traceback(0), scenegraph(nullptr), L(luaL_newstate()) { }

	LuaMachine::~LuaMachine()
	{
		R_ASSERT(!this->L,
		         "LuaMachine must be terminated before the graphics context");
	}

	void LuaMachine::close()
	{
		if (!this->L)
			return;

		Lua::SceneGraph::destroy(this->L, this->scenegraph);
		lua_close(this->L);
		this->L = nullptr;
	}

	int LuaMachine::init(SceneGraph::Node *root)
	{
		luaR_openlibs(this->L);

		// Initialize "rainbow" namespace.
		lua_createtable(this->L, 0, 16);
		Lua::init(this->L);

		// Set "rainbow.breakpoint".
		luaR_rawsetcclosurefield(L, breakpoint, "breakpoint");

		// Set "rainbow.time_since_epoch".
		luaR_rawsetcclosurefield(L, time_since_epoch, "time_since_epoch");

		// Initialize "rainbow.scenegraph".
		this->scenegraph = Lua::SceneGraph::create(this->L, root);

		// Bind C++ objects.
		Lua::bind(this->L);

		const char rainbow[] = "rainbow";
		lua_setglobal(this->L, rainbow);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

		// Set up custom loader.
		lua_getglobal(this->L, "package");
		R_ASSERT(!lua_isnil(this->L, -1), "package table does not exist");
		lua_pushliteral(this->L, "searchers");
		lua_rawget(this->L, -2);
		R_ASSERT(!lua_isnil(this->L, -1),
		         "package.searchers table does not exist");

		// Make space in the second slot for our loader.
		for (size_t i = lua_rawlen(this->L, -1); i > 1; --i)
		{
			lua_rawgeti(this->L, -1, i);
			lua_rawseti(this->L, -2, i + 1);
		}
		lua_pushcfunction(this->L, Lua::load);
		lua_rawseti(this->L, -2, 2);
		lua_pop(this->L, 2);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

		// Load our internal script.
		const size_t length = sizeof(Rainbow_lua) / sizeof(char) - 1;
		if (Lua::load(this->L,
		              Data(Rainbow_lua, length, Data::kDataReference),
		              rainbow) == 0)
		{
			return luaL_error(this->L, "Failed to load internal Lua script");
		}
		lua_getglobal(this->L, "__update");
		R_ASSERT(lua_isfunction(this->L, -1),
		         "Failed to get internal Lua script");
		this->internal = luaL_ref(this->L, LUA_REGISTRYINDEX);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

	#ifndef NDEBUG
		lua_getglobal(this->L, "debug");
		lua_pushliteral(this->L, "traceback");
		lua_rawget(this->L, -2);
		this->traceback = luaL_ref(this->L, LUA_REGISTRYINDEX);
		lua_pop(this->L, 1);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");
	#endif

		// Map 'lua_createtable' to 'table.create'
		lua_getglobal(this->L, "table");
		lua_pushliteral(this->L, "create");
		lua_pushcfunction(this->L, createtable);
		lua_rawset(this->L, -3);
		lua_pop(this->L, 1);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

		return LUA_OK;
	}
}
