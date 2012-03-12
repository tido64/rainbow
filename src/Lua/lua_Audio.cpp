#include "LuaMachine.h"
#include "Common/Data.h"
#include "Common/RainbowAssert.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Wave.h"
#include "Lua/lua_Audio.h"
#include "Lua/lua_Recorder.h"

using ConFuoco::Mixer;
using ConFuoco::Wave;

void lua_Audio::init(lua_State *L)
{
	lua_createtable(L, 0, 32);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "audio");

	lua_pushcclosure(L, &lua_Audio::set_gain, 0);
	lua_setfield(L, -2, "set_gain");

	lua_pushcclosure(L, &lua_Audio::set_orientation, 0);
	lua_setfield(L, -2, "set_orientation");

	lua_pushcclosure(L, &lua_Audio::set_pitch, 0);
	lua_setfield(L, -2, "set_pitch");

	lua_pushcclosure(L, &lua_Audio::clear, 0);
	lua_setfield(L, -2, "clear");

	lua_pushcclosure(L, &lua_Audio::load_sfx, 0);
	lua_setfield(L, -2, "load_sfx");

	lua_pushcclosure(L, &lua_Audio::load_stream, 0);
	lua_setfield(L, -2, "load_stream");

	lua_pushcclosure(L, &lua_Audio::pause, 0);
	lua_setfield(L, -2, "pause");

	lua_pushcclosure(L, &lua_Audio::play, 0);
	lua_setfield(L, -2, "play");

	lua_pushcclosure(L, &lua_Audio::stop, 0);
	lua_setfield(L, -2, "stop");

	LuaMachine::wrap<lua_Recorder>(L);

	lua_pop(L, 1);
}

int lua_Audio::set_gain(lua_State *L)
{
	Mixer::Instance().set_gain(lua_tonumber(L, 1));
	return 0;
}

int lua_Audio::set_orientation(lua_State *L)
{
	Mixer::Instance().set_orientation(lua_tonumber(L, 1));
	return 0;
}

int lua_Audio::set_pitch(lua_State *L)
{
	Mixer::Instance().set_pitch(lua_tonumber(L, 1));
	return 0;
}

int lua_Audio::clear(lua_State *)
{
	Mixer::Instance().clear();
	return 0;
}

int lua_Audio::load_sfx(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.load_sfx(file)");

	const char *file = lua_tolstring(L, 1, nullptr);
	const char *path = Data::get_path(file);
	lua_pushlightuserdata(L, Mixer::Instance().load_sfx(path));
	Data::free(path);
	return 1;
}

int lua_Audio::load_stream(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.load_stream(file)");

	const char *file = lua_tolstring(L, 1, nullptr);
	const char *path = Data::get_path(file);
	lua_pushlightuserdata(L, Mixer::Instance().load_stream(path));
	Data::free(path);
	return 1;
}

int lua_Audio::pause(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.pause(id)");

	static_cast<Wave*>(lua_touserdata(L, 1))->pause();
	return 0;
}

int lua_Audio::play(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.play(id)");

	static_cast<Wave*>(lua_touserdata(L, 1))->play();
	return 0;
}

int lua_Audio::stop(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.stop(id)");

	static_cast<Wave*>(lua_touserdata(L, 1))->stop();
	return 0;
}
