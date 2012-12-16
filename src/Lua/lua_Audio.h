#ifndef LUA_AUDIO_H_
#define LUA_AUDIO_H_

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		namespace Audio
		{
			void init(lua_State *);

			int set_gain(lua_State *);
			int set_orientation(lua_State *);
			int set_pitch(lua_State *);

			int clear(lua_State *);
			int create_sound(lua_State *);
			int delete_sound(lua_State *);

			int pause(lua_State *);
			int play(lua_State *);
			int stop(lua_State *);
		}
	}
}

#endif
