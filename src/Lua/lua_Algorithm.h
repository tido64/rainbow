#ifndef LUA_ALGORITHM_H_
#define LUA_ALGORITHM_H_

#include "../Algorithm.h"
#include "../Lua.h"

class lua_Algorithm
{
public:
	lua_Algorithm(lua_State *);
	static int random(lua_State *);
};

#endif
