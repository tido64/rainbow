#include "Lua/lua_Recorder.h"

const char lua_Recorder::class_name[] = "recorder";
const LuaMachine::Method<lua_Recorder> lua_Recorder::methods[] = {
	{ "get_average_power", &lua_Recorder::get_average_power },
	{ "get_low_pass",      &lua_Recorder::get_low_pass },
	{ "get_peak_power",    &lua_Recorder::get_peak_power },
	{ "pause",             &lua_Recorder::pause },
	{ "record",            &lua_Recorder::record },
	{ "stop",              &lua_Recorder::stop },
	{ "update",            &lua_Recorder::update },
	{ 0, 0 }
};

lua_Recorder::lua_Recorder(lua_State *L)
{
	if (!this->recorder)
		luaL_error(L, "rainbow.audio.recorder: Failed to initialise recorder");
}

int lua_Recorder::get_average_power(lua_State *L)
{
	lua_pushnumber(L, this->recorder.get_average_power());
	return 1;
}

int lua_Recorder::get_low_pass(lua_State *L)
{
	lua_pushnumber(L, this->recorder.get_low_pass());
	return 1;
}

int lua_Recorder::get_peak_power(lua_State *L)
{
	lua_pushnumber(L, this->recorder.get_peak_power());
	return 1;
}
