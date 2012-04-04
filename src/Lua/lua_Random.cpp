#include <lua.hpp>

#include "Common/Random.h"
#include "Lua/lua_Random.h"

void lua_Random::init(lua_State *L)
{
	lua_pushcclosure(L, &lua_Random::random, 0);
	lua_setfield(L, -2, "random");
	assert(lua_gettop(L));
}

int lua_Random::random(lua_State *L)
{
	float r = 0.0f;
	switch (lua_gettop(L))
	{
		case 1:
			r = Random::Instance().next<float_p>(lua_tonumber(L, 1));
			break;
		case 2:
			r = lua_tonumber(L, 1);
			r = Random::Instance().next<float_p>(r, lua_tonumber(L, 2));
			break;
		default:
			r = Random::Instance().next();
			break;
	}
	lua_pushnumber(L, r);
	return 1;
}
