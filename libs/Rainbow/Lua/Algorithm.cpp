#include "Algorithm.h"

lua_Algorithm::lua_Algorithm(lua_State *L, const int ns)
{
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, ns, "algorithm");

	lua_pushcclosure(L, &lua_Algorithm::random, 0);
	lua_setfield(L, -2, "random");

	lua_pop(L, 1);
}

int lua_Algorithm::random(lua_State *L)
{
	float r = 0.0f;
	switch (lua_gettop(L))
	{
		case 1:
			r = Rainbow::mt_random() * lua_tonumber(L, 1);
			break;
		case 2:
			r = lua_tonumber(L, 1);
			r = (lua_tonumber(L, 2) - r) * Rainbow::mt_random() + r;
			break;
		default:
			r = Rainbow::mt_random();
			break;
	}
	lua_pushnumber(L, r);
	return 1;
}
