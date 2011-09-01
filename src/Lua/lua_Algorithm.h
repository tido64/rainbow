#ifndef LUA_ALGORITHM_H_
#define LUA_ALGORITHM_H_

#include "lua.h"
#include "Algorithm.h"

class lua_Algorithm
{
public:
	static void init(lua_State *);

	static int angle(lua_State *);
	static int deg2rad(lua_State *);
	static int rad2deg(lua_State *);
	static int random(lua_State *);

private:
	lua_Algorithm();
	lua_Algorithm(const lua_Algorithm &);
	lua_Algorithm& operator=(const lua_Algorithm &);
};

#endif
