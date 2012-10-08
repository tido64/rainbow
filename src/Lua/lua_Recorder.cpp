#include "Lua/lua_Recorder.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Recorder::class_name[] = "recorder";
		const LuaMachine::Method<Recorder> Recorder::methods[] = {
			{ "get_average_power", &Recorder::get_average_power },
			{ "get_low_pass",      &Recorder::get_low_pass },
			{ "get_peak_power",    &Recorder::get_peak_power },
			{ "pause",             &Recorder::pause },
			{ "record",            &Recorder::record },
			{ "stop",              &Recorder::stop },
			{ "update",            &Recorder::update },
			{ 0, 0 }
		};

		Recorder::Recorder(lua_State *L)
		{
			if (!this->recorder)
				luaL_error(L, "rainbow.audio.recorder: Failed to initialise recorder");
		}

		int Recorder::get_average_power(lua_State *L)
		{
			lua_pushnumber(L, this->recorder.get_average_power());
			return 1;
		}

		int Recorder::get_low_pass(lua_State *L)
		{
			lua_pushnumber(L, this->recorder.get_low_pass());
			return 1;
		}

		int Recorder::get_peak_power(lua_State *L)
		{
			lua_pushnumber(L, this->recorder.get_peak_power());
			return 1;
		}
	}
}
