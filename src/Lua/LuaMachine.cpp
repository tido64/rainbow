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
		using Rainbow::Lua::optinteger;
		lua_createtable(L, optinteger(L, 1, 0), optinteger(L, 2, 0));
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
		lua_pushunsigned(L, Chrono::time_since_epoch().count());
		return 1;
	}
}

namespace Rainbow
{
	int LuaMachine::start(const Data &main)
	{
		if (Lua::load(state_, main, "main") == 0)
			return luaL_error(state_, "Failed to load main script");

	#ifndef NDEBUG
		lua_rawgeti(state_, LUA_REGISTRYINDEX, traceback_);
	#endif
		lua_getglobal(state_, "init");
	#ifndef NDEBUG
		return Lua::call(state_, 0, 0, 1, "Failed to initialise main script");
	#else
		return Lua::call(state_, 0, 0, 0, "Failed to initialise main script");
	#endif
	}

	int LuaMachine::update(const unsigned long t)
	{
	#ifndef NDEBUG
		lua_rawgeti(state_, LUA_REGISTRYINDEX, traceback_);
	#endif
		lua_rawgeti(state_, LUA_REGISTRYINDEX, internal_);
		lua_pushunsigned(state_, t);
	#ifndef NDEBUG
		return Lua::call(state_, 1, 0, 1, "Failed to call 'update'");
	#else
		return Lua::call(state_, 1, 0, 0, "Failed to call 'update'");
	#endif
	}

	LuaMachine::LuaMachine()
	    : state_(luaL_newstate()), internal_(0), traceback_(0),
	      scenegraph_(nullptr) { }

	LuaMachine::~LuaMachine()
	{
		R_ASSERT(!state_,
		         "LuaMachine must be terminated before the graphics context");
	}

	void LuaMachine::close()
	{
		if (!state_)
			return;

		Lua::SceneGraph::destroy(state_, scenegraph_);
		lua_close(state_);
		state_ = nullptr;
	}

	int LuaMachine::init(SceneGraph::Node *root)
	{
		luaR_openlibs(state_);

		// Initialize "rainbow" namespace.
		lua_createtable(state_, 0, 16);
		Lua::init(state_);

		// Set "rainbow.breakpoint".
		luaR_rawsetcfunction(state_, "breakpoint", breakpoint);

		// Set "rainbow.time_since_epoch".
		luaR_rawsetcfunction(state_, "time_since_epoch", time_since_epoch);

		// Initialize "rainbow.scenegraph".
		scenegraph_ = Lua::SceneGraph::create(state_, root);

		// Bind C++ objects.
		Lua::bind(state_);

		const char rainbow[] = "rainbow";
		lua_setglobal(state_, rainbow);
		R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

		// Set up custom loader.
		lua_getglobal(state_, "package");
		R_ASSERT(!lua_isnil(state_, -1), "package table does not exist");
		lua_pushliteral(state_, "searchers");
		lua_rawget(state_, -2);
		R_ASSERT(!lua_isnil(state_, -1),
		         "package.searchers table does not exist");

		// Make space in the top slot for our loader.
		for (size_t i = lua_rawlen(state_, -1); i > 0; --i)
		{
			lua_rawgeti(state_, -1, i);
			lua_rawseti(state_, -2, i + 1);
		}
		lua_pushcfunction(state_, Lua::load);
		lua_rawseti(state_, -2, 1);
		lua_pop(state_, 2);
		R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

		// Load our internal script.
		if (Lua::load(state_, Data::from_literal(Rainbow_lua), rainbow) == 0)
			return luaL_error(state_, "Failed to load internal Lua script");
		lua_getglobal(state_, "__update");
		R_ASSERT(lua_isfunction(state_, -1),
		         "Failed to get internal Lua script");
		internal_ = luaL_ref(state_, LUA_REGISTRYINDEX);
		R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

	#ifndef NDEBUG
		lua_getglobal(state_, "debug");
		lua_pushliteral(state_, "traceback");
		lua_rawget(state_, -2);
		traceback_ = luaL_ref(state_, LUA_REGISTRYINDEX);
		lua_pop(state_, 1);
		R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");
	#endif

		// Map 'lua_createtable' to 'table.create'
		lua_getglobal(state_, "table");
		lua_pushliteral(state_, "create");
		lua_pushcfunction(state_, createtable);
		lua_rawset(state_, -3);
		lua_pop(state_, 1);
		R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

		return LUA_OK;
	}
}
