#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Sound.h"
#include "Lua/lua_Audio.h"
#include "Lua/lua_Recorder.h"

using ConFuoco::Channel;
using ConFuoco::Mixer;
using ConFuoco::Sound;

namespace Rainbow
{
	namespace Lua
	{
		namespace Audio
		{
			namespace
			{
				const char channel_type[] = "channel";
				const char sound_type[] = "sound";
			}

			void init(lua_State *L)
			{
				lua_pushliteral(L, "audio");
				lua_createtable(L, 0, 32);

				lua_rawsetcclosurefield(L, &set_gain, 0, "set_gain");
				lua_rawsetcclosurefield(L, &set_pitch, 0, "set_pitch");
				lua_rawsetcclosurefield(L, &clear, 0, "clear");
				lua_rawsetcclosurefield(L, &create_sound, 0, "create_sound");
				lua_rawsetcclosurefield(L, &delete_sound, 0, "delete_sound");
				lua_rawsetcclosurefield(L, &pause, 0, "pause");
				lua_rawsetcclosurefield(L, &play, 0, "play");
				lua_rawsetcclosurefield(L, &stop, 0, "stop");

				wrap<Recorder>(L);

				lua_rawset(L, -3);
			}

			int set_gain(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2,
				           "rainbow.audio.set_gain([source,] volume)");

				if (lua_gettop(L) == 2)
				{
					const float gain = lua_tonumber(L, 2);
					lua_pop(L, 1);
					Channel *ch = static_cast<Channel*>(topointer(L, channel_type));
					ch->set_gain(gain);
				}
				else
					Mixer::Instance->set_gain(lua_tonumber(L, 1));
				return 0;
			}

			int set_pitch(lua_State *L)
			{
				Mixer::Instance->set_pitch(lua_tonumber(L, 1));
				return 0;
			}

			int clear(lua_State *)
			{
				Mixer::Instance->clear();
				return 0;
			}

			int create_sound(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) >= 1 && lua_gettop(L) <= 3, "rainbow.audio.create_sound(file[, type, loops])");

				const char *file = lua_tolstring(L, 1, nullptr);
				const int type = (lua_gettop(L) >= 2) ? lua_tointeger(L, 2) : ConFuoco::STATIC;
				const int loops = (lua_gettop(L) == 3) ? lua_tointeger(L, 3) : -1;
				pushpointer(L, Mixer::Instance->create_sound(file, type, loops), sound_type);
				return 1;
			}

			int delete_sound(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.delete_sound(sound)");

				delete static_cast<Sound*>(topointer(L, sound_type));
				return 0;
			}

			int pause(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.pause(channel)");

				Channel *ch = static_cast<Channel*>(topointer(L, channel_type));
				ch->pause();
				return 0;
			}

			int play(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.play(sound)");

				Sound *snd = static_cast<Sound*>(topointer(L, sound_type));
				pushpointer(L, Mixer::Instance->play(snd), channel_type);
				return 1;
			}

			int stop(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.stop(channel)");

				Channel *ch = static_cast<Channel*>(topointer(L, channel_type));
				ch->stop();
				return 0;
			}
		}
	}
}
