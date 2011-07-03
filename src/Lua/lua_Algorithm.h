#ifndef LUA_ALGORITHM_H_
#define LUA_ALGORITHM_H_

#include "lua.h"
#include "Algorithm.h"

class lua_Algorithm
{
public:
	lua_Algorithm(lua_State *);

	static int angle(lua_State *);
	static int deg2rad(lua_State *);
	static int rad2deg(lua_State *);
	static int random(lua_State *);
};

#endif
