#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "Algorithm.h"
#include "LuaMachine.h"
#include "Input/Input.h"
#include "Input/Key.h"
#include "Input/Touch.h"
#include "Lua/lua_Input.h"

void lua_Input::init(lua_State *L)
{
	lua_createtable(L, 0, 2);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "input");

	// rainbow.input.acceleration
	lua_createtable(L, 0, 2);
	lua_setfield(L, -2, "acceleration");
	accelerate(L);

	lua_pop(L, 1);
}

void lua_Input::accelerate(lua_State *L)
{
	const Input &input = Input::Instance();
	getfield(L, "acceleration");
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

void lua_Input::getfield(lua_State *L, const char *const field)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
	lua_getfield(L, -1, "input");
	lua_getfield(L, -1, field);
}

void lua_Input::touch_event(lua_State *L, const char *const type, const Touch *const touches, const unsigned int count)
{
	getfield(L, type);
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}

	lua_createtable(L, 0, count);
	for (unsigned int i = 0; i < count; ++i)
	{
		lua_createtable(L, 0, 2);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, Rainbow::itoa(touches[i].hash));
		lua_pushinteger(L, touches[i].x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, touches[i].y);
		lua_setfield(L, -2, "y");
		lua_pop(L, 1);
	}
	lua_call(L, 1, 0);
	lua_pop(L, 2);
	assert(lua_gettop(L) == 0 || !"Rainbow::Lua::Input::touch_event: Leftover elements in the stack");
}

void lua_Input::touch_began(lua_State *L, const Touch *const touches, const unsigned int count)
{
	touch_event(L, "touch_began", touches, count);
}

void lua_Input::touch_canceled(lua_State *L)
{
	getfield(L, "touch_canceled");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}

	lua_call(L, 0, 0);
	lua_pop(L, 2);
	assert(lua_gettop(L) == 0 || !"Rainbow::Lua::Input::touch_canceled: Leftover elements in the stack");
}

void lua_Input::touch_ended(lua_State *L, const Touch *const touches, const unsigned int count)
{
	touch_event(L, "touch_ended", touches, count);
}

void lua_Input::touch_moved(lua_State *L, const Touch *const touches, const unsigned int count)
{
	touch_event(L, "touch_moved", touches, count);
}

#ifdef RAINBOW_BUTTONS

void lua_Input::key_event(lua_State *L, const char *const type, const Key &key)
{
	getfield(L, type);
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 3);
		return;
	}

	lua_pushinteger(L, key.key);
	lua_pushinteger(L, key.modifier);
	lua_call(L, 2, 0);
	lua_pop(L, 2);
	assert(lua_gettop(L) == 0 || !"Rainbow::Lua::Input::key_event: Leftover elements in the stack");
}

void lua_Input::key_down(lua_State *L, const Key &key)
{
	key_event(L, "key_down", key);
}

void lua_Input::key_up(lua_State *L, const Key &key)
{
	key_event(L, "key_up", key);
}

#endif  // RAINBOW_BUTTONS
