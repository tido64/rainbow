#include "Common/Data.h"
#include "Common/Debug.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Stream.h"
#include "Lua/lua_Audio.h"
#include "Lua/lua_Recorder.h"

using ConFuoco::Mixer;
using ConFuoco::Sound;
using ConFuoco::Stream;
using ConFuoco::Wave;

namespace Rainbow
{
	namespace Lua
	{
		void Audio::init(lua_State *L)
		{
			lua_createtable(L, 0, 32);
			lua_pushvalue(L, -1);
			lua_setfield(L, -3, "audio");

			lua_pushcclosure(L, &Audio::set_gain, 0);
			lua_setfield(L, -2, "set_gain");

			lua_pushcclosure(L, &Audio::set_orientation, 0);
			lua_setfield(L, -2, "set_orientation");

			lua_pushcclosure(L, &Audio::set_pitch, 0);
			lua_setfield(L, -2, "set_pitch");

			lua_pushcclosure(L, &Audio::clear, 0);
			lua_setfield(L, -2, "clear");

			lua_pushcclosure(L, &Audio::delete_sfx, 0);
			lua_setfield(L, -2, "delete_sfx");

			lua_pushcclosure(L, &Audio::delete_stream, 0);
			lua_setfield(L, -2, "delete_stream");

			lua_pushcclosure(L, &Audio::load_sfx, 0);
			lua_setfield(L, -2, "load_sfx");

			lua_pushcclosure(L, &Audio::load_stream, 0);
			lua_setfield(L, -2, "load_stream");

			lua_pushcclosure(L, &Audio::pause, 0);
			lua_setfield(L, -2, "pause");

			lua_pushcclosure(L, &Audio::play, 0);
			lua_setfield(L, -2, "play");

			lua_pushcclosure(L, &Audio::stop, 0);
			lua_setfield(L, -2, "stop");

			LuaMachine::wrap<lua_Recorder>(L);

			lua_pop(L, 1);
		}

		int Audio::set_gain(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "rainbow.audio.set_gain([source,] volume)");

			if (lua_gettop(L) == 2)
			{
				Wave *w = static_cast<Wave*>(lua_touserdata(L, 1));
				LUA_CHECK(L, w, "rainbow.audio.set_gain: Invalid source specified");
				w->set_gain(lua_tonumber(L, 2));
			}
			else
				Mixer::Instance().set_gain(lua_tonumber(L, 1));
			return 0;
		}

		int Audio::set_orientation(lua_State *L)
		{
			Mixer::Instance().set_orientation(lua_tonumber(L, 1));
			return 0;
		}

		int Audio::set_pitch(lua_State *L)
		{
			Mixer::Instance().set_pitch(lua_tonumber(L, 1));
			return 0;
		}

		int Audio::clear(lua_State *)
		{
			Mixer::Instance().clear();
			return 0;
		}

		int Audio::delete_sfx(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.delete_sfx(source)");

			const Sound *s = static_cast<const Sound*>(lua_topointer(L, 1));
			LUA_CHECK(L, s, "rainbow.audio.delete_sfx: Invalid source specified");
			Mixer::Instance().remove(s);
			return 0;
		}

		int Audio::delete_stream(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.delete_stream(source)");

			const Stream *s = static_cast<const Stream*>(lua_topointer(L, 1));
			LUA_CHECK(L, s, "rainbow.audio.delete_stream: Invalid source specified");
			Mixer::Instance().remove(s);
			return 0;
		}

		int Audio::load_sfx(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "rainbow.audio.load_sfx(file[, instances])");

			const char *file = lua_tolstring(L, 1, nullptr);
			const unsigned int instances = (lua_gettop(L) == 2) ? lua_tointeger(L, 2) : 1;
			lua_pushlightuserdata(L, Mixer::Instance().load_sfx(file, instances));
			return 1;
		}

		int Audio::load_stream(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "rainbow.audio.load_stream(file[, loops])");

			const char *file = lua_tolstring(L, 1, nullptr);
			Stream* s = static_cast<Stream*>(Mixer::Instance().load_stream(file));
			if (lua_gettop(L) == 2)
				s->set_loops(lua_tointeger(L, 2));
			lua_pushlightuserdata(L, s);
			return 1;
		}

		int Audio::pause(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.pause(id)");

			Wave *w = static_cast<Wave*>(lua_touserdata(L, 1));
			LUA_CHECK(L, w, "rainbow.audio.pause: Invalid source specified");
			w->pause();
			return 0;
		}

		int Audio::play(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.play(id)");

			Wave *w = static_cast<Wave*>(lua_touserdata(L, 1));
			LUA_CHECK(L, w, "rainbow.audio.play: Invalid source specified");
			w->play();
			return 0;
		}

		int Audio::stop(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.audio.stop(id)");

			Wave *w = static_cast<Wave*>(lua_touserdata(L, 1));
			LUA_CHECK(L, w, "rainbow.audio.stop: Invalid source specified");
			w->stop();
			return 0;
		}
	}
}
