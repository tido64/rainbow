#include <cstring>

#include "Common/Data.h"
#include "Common/Debug.h"
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
				const char type_field[] = "__type";

				void pushpointer(lua_State *L, void *ptr, const char *name)
				{
				#ifndef NDEBUG
					lua_createtable(L, 1, 1);
					lua_pushlightuserdata(L, ptr);
					lua_rawseti(L, -2, 0);
					lua_pushstring(L, name);
					lua_setfield(L, -2, type_field);
				#else
					static_cast<void>(name);
					lua_pushlightuserdata(L, ptr);
				#endif
				}

				void* topointer(lua_State *L, const char *name)
				{
				#ifndef NDEBUG
					LUA_CHECK(L, !lua_isnil(L, -1), "Parameter is a nil value");
					LUA_CHECK(L, lua_istable(L, -1), "Object is not of type '%s'", name);
					lua_getfield(L, -1, type_field);
					LUA_CHECK(L, memcmp(lua_tolstring(L, -1, nullptr), name, strlen(name)) == 0, "Object is not of type '%s'", name);
					lua_rawgeti(L, -2, 0);
					void *ptr = lua_touserdata(L, -1);
					lua_pop(L, 2);
					return ptr;
				#else
					static_cast<void>(name);
					return lua_touserdata(L, -1);
				#endif
				}
			}

			void init(lua_State *L)
			{
				lua_createtable(L, 0, 32);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "audio");

				lua_pushcclosure(L, &set_gain, 0);
				lua_setfield(L, -2, "set_gain");

				lua_pushcclosure(L, &set_pitch, 0);
				lua_setfield(L, -2, "set_pitch");

				lua_pushcclosure(L, &clear, 0);
				lua_setfield(L, -2, "clear");

				lua_pushcclosure(L, &create_sound, 0);
				lua_setfield(L, -2, "create_sound");

				lua_pushcclosure(L, &delete_sound, 0);
				lua_setfield(L, -2, "delete_sound");

				lua_pushcclosure(L, &pause, 0);
				lua_setfield(L, -2, "pause");

				lua_pushcclosure(L, &play, 0);
				lua_setfield(L, -2, "play");

				lua_pushcclosure(L, &stop, 0);
				lua_setfield(L, -2, "stop");

				LuaMachine::wrap<Recorder>(L);

				lua_pop(L, 1);
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
