#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

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
	this->accelerate(L);

#endif

	lua_pop(L, 1);
}

#ifdef RAINBOW_ACCELERATED

void lua_Input::accelerate(lua_State *L)
{
	//if (!input.accelerated)
	//	return;

	lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
	lua_getfield(L, -1, "input");

	const Input &input = Input::Instance();

	lua_getfield(L, -1, "acceleration");
	lua_pushnumber(L, input.acceleration.x * RAINBOW_INV_FIXED_SCALE);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, input.acceleration.y * RAINBOW_INV_FIXED_SCALE);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, input.acceleration.z * RAINBOW_INV_FIXED_SCALE);
	lua_setfield(L, -2, "z");
	lua_pushnumber(L, input.acceleration.timestamp);
	lua_setfield(L, -2, "timestamp");

	lua_pop(L, 3);
}

#endif

void lua_Input::touch_began(lua_State *L, const Touch *const touches, const unsigned int count)
{
	this->touch_event(L, touches, count);
	lua_getfield(L, -1, "touch_began");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}
	lua_pcall(L, 0, 0, 0);
	lua_pop(L, 2);
}

void lua_Input::touch_canceled(lua_State *L)
{
	this->touch_event(L);
	lua_getfield(L, -1, "touch_canceled");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}
	lua_pcall(L, 0, 0, 0);
	lua_pop(L, 2);
}

void lua_Input::touch_ended(lua_State *L, const Touch *const touches, const unsigned int count)
{
	this->touch_event(L, touches, count);
	lua_getfield(L, -1, "touch_ended");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}
	lua_pcall(L, 0, 0, 0);
	lua_pop(L, 2);
}

void lua_Input::touch_event(lua_State *L, const Touch *const touches, const unsigned int count)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
	lua_getfield(L, -1, "input");

	if (count == 0)
	{
		lua_pushnil(L);
		lua_setfield(L, -2, "touches");
	}
	else
	{
		static char hash[sizeof(unsigned int)];
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "touches");
		for (unsigned int i = 0; i < count; ++i)
		{
			lua_createtable(L, 0, 0);
			lua_pushvalue(L, -1);
			sprintf(hash, "%u", touches[i].hash);
			lua_setfield(L, -3, hash);

			lua_pushinteger(L, touches[i].position.x);
			lua_setfield(L, -2, "x");
			lua_pushinteger(L, touches[i].position.y);
			lua_setfield(L, -2, "y");

			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}

	// Don't pop it in here
	//lua_pop(L, 2);
}

void lua_Input::touch_moved(lua_State *L, const Touch *const touches, const unsigned int count)
{
	this->touch_event(L, touches, count);
	lua_getfield(L, -1, "touch_moved");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}
	lua_pcall(L, 0, 0, 0);
	lua_pop(L, 2);
}
