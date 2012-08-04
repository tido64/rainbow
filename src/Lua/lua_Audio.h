#ifndef LUA_AUDIO_H_
#define LUA_AUDIO_H_

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		class Audio
		{
			friend void init(lua_State *);

		public:
			static int set_gain(lua_State *);
			static int set_orientation(lua_State *);
			static int set_pitch(lua_State *);

			static int clear(lua_State *);
			static int delete_sfx(lua_State *);
			static int delete_stream(lua_State *);

			static int load_sfx(lua_State *);
			static int load_stream(lua_State *);

			static int pause(lua_State *);
			static int play(lua_State *);
			static int stop(lua_State *);

		private:
			static void init(lua_State *);

			Audio();
			Audio(const Audio &);
			Audio& operator=(const Audio &);
		};
	}
}

#endif
