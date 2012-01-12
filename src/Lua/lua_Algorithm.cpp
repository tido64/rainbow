#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include <lauxlib.h>

#include "Algorithm.h"
#include "Lua/lua_Algorithm.h"

void lua_Algorithm::init(lua_State *L)
{
	lua_createtable(L, 0, 4);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "algorithm");

	lua_pushcclosure(L, &lua_Algorithm::angle, 0);
	lua_setfield(L, -2, "angle");

	lua_pushcclosure(L, &lua_Algorithm::deg2rad, 0);
	lua_setfield(L, -2, "deg2rad");

	lua_pushcclosure(L, &lua_Algorithm::rad2deg, 0);
	lua_setfield(L, -2, "rad2deg");

	lua_pushcclosure(L, &lua_Algorithm::random, 0);
	lua_setfield(L, -2, "random");

	lua_pop(L, 1);
}

int lua_Algorithm::angle(lua_State *L)
{
	lua_pushnumber(L, Rainbow::angle(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
	return 1;
}

int lua_Algorithm::deg2rad(lua_State *L)
{
	lua_pushnumber(L, Rainbow::deg2rad(lua_tonumber(L, 1)));
	return 1;
}

int lua_Algorithm::rad2deg(lua_State *L)
{
	lua_pushnumber(L, Rainbow::rad2deg(lua_tonumber(L, 1)));
	return 1;
}

int lua_Algorithm::random(lua_State *L)
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
