// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Lua/lua_Recorder.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Recorder::Bind::class_name[] = "recorder";

	template<>
	const Method<Recorder> Recorder::Bind::methods[] = {
		{ "get_average_power", &Recorder::get_average_power },
		{ "get_low_pass",      &Recorder::get_low_pass },
		{ "get_peak_power",    &Recorder::get_peak_power },
		{ "pause",             &Recorder::pause },
		{ "record",            &Recorder::record },
		{ "stop",              &Recorder::stop },
		{ "update",            &Recorder::update },
		{ 0, 0 }
	};

	Recorder::Recorder(lua_State *) { }

	int Recorder::get_average_power(lua_State *L)
	{
		lua_pushnumber(L, ConFuoco::Recorder::average_power());
		return 1;
	}

	int Recorder::get_low_pass(lua_State *L)
	{
		lua_pushnumber(L, ConFuoco::Recorder::low_pass());
		return 1;
	}

	int Recorder::get_peak_power(lua_State *L)
	{
		lua_pushnumber(L, ConFuoco::Recorder::peak_power());
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
} NS_RAINBOW_LUA_END
