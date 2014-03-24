// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Sound.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Audio.h"
#include "Lua/lua_Recorder.h"

using ConFuoco::Mixer;
using ConFuoco::Sound;

namespace
{
	const char kChannelType[] = "channel";
	const char kSoundType[] = "sound";

	Mixer::Channel* tochannel(lua_State *L)
	{
		return static_cast<Mixer::Channel*>(
		    Rainbow::Lua::topointer(L, kChannelType));
	}

	int set_gain(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2,
		           "rainbow.audio.set_gain([source,] volume)");

		if (lua_gettop(L) == 2)
		{
			const float gain = luaR_tonumber(L, 2);
			lua_pop(L, 1);

			Mixer::Channel *ch = tochannel(L);
			if (!ch)
				return 0;

			Mixer::Instance->set_gain(ch, gain);
		}
		else
			Mixer::Instance->set_gain(luaR_tonumber(L, 1));
		return 0;
	}

	int set_pitch(lua_State *L)
	{
		Mixer::Instance->set_pitch(luaR_tonumber(L, 1));
		return 0;
	}

	int clear(lua_State *)
	{
		Mixer::Instance->clear();
		return 0;
	}

	int create_sound(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) >= 1 && lua_gettop(L) <= 3,
		           "rainbow.audio.create_sound(file[, type, loops])");

		const char *file = luaR_tostring(L, 1);
		const int type =
		    luaR_optinteger(L, 2, static_cast<int>(Sound::Type::Static));
		const int loops = luaR_optinteger(L, 3, -1);
		Rainbow::Lua::pushpointer(
		    L,
		    Mixer::Instance->create_sound(
		        file, static_cast<Sound::Type>(type), loops),
		    kSoundType);
		return 1;
	}

	int delete_sound(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.delete_sound(sound)");

		delete static_cast<Sound*>(Rainbow::Lua::topointer(L, kSoundType));
		return 0;
	}

	int pause(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.pause(channel)");

		Mixer::Channel *ch = tochannel(L);
		if (!ch)
			return 0;

		Mixer::Instance->pause(ch);
		return 0;
	}

	int play(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.play(sound)");

		Sound *snd =
		    static_cast<Sound*>(Rainbow::Lua::topointer(L, kSoundType));
		Rainbow::Lua::pushpointer(L, Mixer::Instance->play(snd), kChannelType);
		return 1;
	}

	int stop(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.stop(channel)");

		Mixer::Channel *ch = tochannel(L);
		if (!ch)
			return 0;

		Mixer::Instance->stop(ch);
		return 0;
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(Audio)
{
	void init(lua_State *L)
	{
		lua_pushliteral(L, "audio");
		lua_createtable(L, 0, 16);

		luaR_rawsetfield(L,
		                 lua_pushinteger,
		                 static_cast<int>(Sound::Type::Static),
		                 "STATIC");
		luaR_rawsetfield(L,
		                 lua_pushinteger,
		                 static_cast<int>(Sound::Type::Stream),
		                 "STREAM");

		luaR_rawsetcclosurefield(L, &set_gain, "set_gain");
		luaR_rawsetcclosurefield(L, &set_pitch, "set_pitch");
		luaR_rawsetcclosurefield(L, &clear, "clear");
		luaR_rawsetcclosurefield(L, &create_sound, "create_sound");
		luaR_rawsetcclosurefield(L, &delete_sound, "delete_sound");
		luaR_rawsetcclosurefield(L, &pause, "pause");
		luaR_rawsetcclosurefield(L, &play, "play");
		luaR_rawsetcclosurefield(L, &stop, "stop");

		wrap<Recorder>(L);

		lua_rawset(L, -3);
	}
} NS_RAINBOW_LUA_MODULE_END(Audio)
