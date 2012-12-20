// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Lua/Modules.h"

namespace Rainbow
{
	LuaMachine::~LuaMachine()
	{
		delete this->scenegraph;
		lua_close(this->L);
	}

	int LuaMachine::update(const unsigned long t)
	{
		lua_pushinteger(this->L, t);
		return Lua::call(this->L, "update", 1);
	}

	LuaMachine::LuaMachine(SceneGraph::Node *root) :
		scenegraph(nullptr), L(luaL_newstate())
	{
		luaL_openlibs(this->L);

		// Initialize "rainbow" namespace
		lua_createtable(this->L, 0, 16);
		lua_pushvalue(this->L, -1);
		lua_setglobal(this->L, "rainbow");
		Lua::init(this->L);

		// Initialize "rainbow.scenegraph"
		this->scenegraph = new Lua::SceneGraph(this->L, root);

		// Bind C++ objects
		Lua::bind(this->L);

		// Clean up the stack
		lua_pop(this->L, 1);
		R_ASSERT(lua_gettop(this->L) == 0, "Stack not empty");

		// Set up custom loader
		lua_getglobal(this->L, "package");
		R_ASSERT(!lua_isnil(this->L, -1), "package table does not exist");
		lua_getfield(this->L, -1, "searchers");
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
	}
}
