#include "Lua/LuaHelper.h"
#include "Lua/lua_Recorder.h"

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<Recorder, ConFuoco::Recorder, kBindTypeDerived> LuaRecorder;

		template<>
		const char LuaRecorder::class_name[] = "recorder";

		template<>
		const Method<Recorder> LuaRecorder::methods[] = {
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
			if (!*this)
				luaL_error(L, "rainbow.audio.recorder: Failed to initialise recorder");
		}

		int Recorder::get_average_power(lua_State *L)
		{
			lua_pushnumber(L, ConFuoco::Recorder::get_average_power());
			return 1;
		}

		int Recorder::get_low_pass(lua_State *L)
		{
			lua_pushnumber(L, ConFuoco::Recorder::get_low_pass());
			return 1;
		}

		int Recorder::get_peak_power(lua_State *L)
		{
			lua_pushnumber(L, ConFuoco::Recorder::get_peak_power());
			return 1;
		}

		int Recorder::pause(lua_State *)
		{
			ConFuoco::Recorder::pause();
			return 0;
		}

		int Recorder::record(lua_State *)
		{
			ConFuoco::Recorder::record();
			return 0;
		}

		int Recorder::stop(lua_State *)
		{
			ConFuoco::Recorder::stop();
			return 0;
		}

		int Recorder::update(lua_State *)
		{
			ConFuoco::Recorder::update();
			return 0;
		}
	}
}
