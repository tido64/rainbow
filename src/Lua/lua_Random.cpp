// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Random.h"

#include <lua.hpp>

#include "Common/Random.h"

namespace
{
	int random(lua_State *L)
	{
		lua_Number r = 0.0f;
		switch (lua_gettop(L))
		{
			case 1:
				LUA_ASSERT(lua_isnumber(L, 1), "rainbow.random(max)");
				r = Random::next<lua_Number>(lua_tonumber(L, 1));
				break;
			case 2:
				LUA_ASSERT(lua_isnumber(L, 1) && lua_isnumber(L, 2),
				           "rainbow.random(min, max)");
				r = Random::next<lua_Number>(lua_tonumber(L, 1),
				                             lua_tonumber(L, 2));
				break;
			default:
				r = Random::next();
				break;
		}
		lua_pushnumber(L, r);
		return 1;
	}

	int seed(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 1) || lua_isnone(L, 1),
		           "rainbow.seed(seed = 0)");

		Random::seed(luaR_optinteger(L, 1, 0));
		return 0;
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(Random)
{
	void init(lua_State *L)
	{
		luaR_rawsetcclosurefield(L, &random, "random");
		luaR_rawsetcclosurefield(L, &seed, "seed");
	}
} NS_RAINBOW_LUA_MODULE_END(Random)
