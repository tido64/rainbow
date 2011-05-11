#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "lua_Audio.h"

using Rainbow::ConFuoco::Mixer;

lua_Audio::lua_Audio(lua_State *L)
{
	lua_createtable(L, 0, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "audio");

	lua_pushcclosure(L, &lua_Audio::set_doppler_factor, 0);
	lua_setfield(L, -2, "set_doppler_factor");

	lua_pushcclosure(L, &lua_Audio::set_doppler_velocity, 0);
	lua_setfield(L, -2, "set_doppler_velocity");

	lua_pushcclosure(L, &lua_Audio::set_speed_of_sound, 0);
	lua_setfield(L, -2, "set_speed_of_sound");

	lua_pushcclosure(L, &lua_Audio::set_gain, 0);
	lua_setfield(L, -2, "set_gain");

	lua_pushcclosure(L, &lua_Audio::set_orientation, 0);
	lua_setfield(L, -2, "set_orientation");

	lua_pushcclosure(L, &lua_Audio::set_pitch, 0);
	lua_setfield(L, -2, "set_pitch");

	lua_pushcclosure(L, &lua_Audio::set_position, 0);
	lua_setfield(L, -2, "set_position");

	lua_pushcclosure(L, &lua_Audio::set_velocity, 0);
	lua_setfield(L, -2, "set_velocity");

	lua_pushcclosure(L, &lua_Audio::add, 0);
	lua_setfield(L, -2, "add");

	lua_pushcclosure(L, &lua_Audio::play, 0);
	lua_setfield(L, -2, "play");

	lua_pushcclosure(L, &lua_Audio::set_looping, 0);
	lua_setfield(L, -2, "set_looping");

	lua_pushcclosure(L, &lua_Audio::set_bgm, 0);
	lua_setfield(L, -2, "set_bgm");

	lua_pushcclosure(L, &lua_Audio::play_bgm, 0);
	lua_setfield(L, -2, "play_bgm");

	lua_pushcclosure(L, &lua_Audio::pause_bgm, 0);
	lua_setfield(L, -2, "pause_bgm");

	lua_pushcclosure(L, &lua_Audio::stop_bgm, 0);
	lua_setfield(L, -2, "stop_bgm");

	lua_pushcclosure(L, &lua_Audio::set_bgm_pan, 0);
	lua_setfield(L, -2, "set_bgm_pan");

	lua_pushcclosure(L, &lua_Audio::set_bgm_volume, 0);
	lua_setfield(L, -2, "set_bgm_volume");

	lua_pop(L, 1);
}

int lua_Audio::set_doppler_factor(lua_State *L)
{
	Mixer::Instance().set_doppler_factor(lua_tonumber(L, 1));
	return 0;
}

int lua_Audio::set_doppler_velocity(lua_State *L)
{
	Mixer::Instance().set_doppler_velocity(lua_tonumber(L, 1));
	return 0;
}

int lua_Audio::set_speed_of_sound(lua_State *L)
{
	Mixer::Instance().set_speed_of_sound(lua_tonumber(L, 1));
	return 0;
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

int lua_Audio::set_position(lua_State *L)
{
	if (lua_gettop(L) == 4)
		Mixer::Instance().set_position(lua_tointeger(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
	else
		Mixer::Instance().set_position(static_cast<float>(lua_tonumber(L, 1)), lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

int lua_Audio::set_velocity(lua_State *L)
{
	if (lua_gettop(L) == 4)
		Mixer::Instance().set_velocity(lua_tointeger(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
	else
		Mixer::Instance().set_velocity(static_cast<float>(lua_tonumber(L, 1)), lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

int lua_Audio::add(lua_State *L)
{
	lua_pushinteger(L, Mixer::Instance().add(AssetManager::Instance().get_full_path(lua_tolstring(L, 1, 0))));
	return 1;
}

int lua_Audio::play(lua_State *L)
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	switch (lua_gettop(L))
	{
		case 4:
			z = lua_tonumber(L, 4);
		case 3:
			y = lua_tonumber(L, 3);
		case 2:
			x = lua_tonumber(L, 2);
		default:
			break;
	}

	lua_pushinteger(L, Mixer::Instance().play(lua_tointeger(L, 1), x, y, z));
	return 1;
}

int lua_Audio::set_looping(lua_State *L)
{
	Mixer::Instance().set_looping(lua_tointeger(L, 1), lua_toboolean(L, 2) == 1, 0);
	return 0;
}

int lua_Audio::set_bgm(lua_State *L)
{
	Mixer::Instance().set_bgm(AssetManager::Instance().get_full_path(lua_tolstring(L, 1, 0)));
	return 0;
}

int lua_Audio::play_bgm(lua_State *L)
{
	Mixer::Instance().play_bgm();
	return 0;
}

int lua_Audio::pause_bgm(lua_State *L)
{
	Mixer::Instance().pause_bgm();
	return 0;
}

int lua_Audio::stop_bgm(lua_State *L)
{
	Mixer::Instance().stop_bgm();
	return 0;
}

int lua_Audio::set_bgm_pan(lua_State *L)
{
	Mixer::Instance().set_bgm_pan(lua_tonumber(L, 1));
	return 0;
}

int lua_Audio::set_bgm_volume(lua_State *L)
{
	Mixer::Instance().set_bgm_volume(lua_tonumber(L, 1));
	return 0;
}
