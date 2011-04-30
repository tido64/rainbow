#include "lua_Input.h"

void lua_Input::init(lua_State *L)
{
	lua_createtable(L, 0, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "input");

#ifdef RAINBOW_ACCELERATED

	// rainbow.input.acceleration
	lua_createtable(L, 0, 0);
	lua_setfield(L, -2, "acceleration");

#endif

	lua_pop(L, 1);
	this->update(L);
}

void lua_Input::touch_event(lua_State *L)
{
	const Input &input = Input::Instance();

	lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
	lua_getfield(L, -1, "input");

	const Touches &touches = input.get_touches();
	if (touches.size() == 0)
	{
		lua_pushnil(L);
		lua_setfield(L, -2, "touches");
	}
	else
	{
		char hash[32];
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "touches");
		for (Touches::const_iterator i = touches.cbegin(); i != touches.cend(); ++i)
		{
			sprintf(hash, "%u", i->first);

			lua_createtable(L, 0, 0);
			lua_pushvalue(L, -1);
			lua_setfield(L, -3, hash);

			lua_pushnumber(L, i->second.position.x);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, i->second.position.y);
			lua_setfield(L, -2, "y");

			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}

	lua_pop(L, 2);
}

void lua_Input::update(lua_State *L)
{
#ifdef RAINBOW_ACCELERATED

	const Input &input = Input::Instance();

	lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
	lua_getfield(L, -1, "input");

	if (input.accelerated)
	{
		lua_getfield(L, -1, "acceleration");
		lua_pushnumber(L, input.acceleration.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, input.acceleration.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, input.acceleration.z);
		lua_setfield(L, -2, "z");
		lua_pushnumber(L, input.acceleration.timestamp);
		lua_setfield(L, -2, "timestamp");
		lua_pop(L, 1);
	}

	lua_pop(L, 2);

#endif
}
