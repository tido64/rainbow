#ifndef LUA_AUDIO_H_
#define LUA_AUDIO_H_

struct lua_State;

class lua_Audio
{
	friend class LuaMachine;

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

	lua_Audio();
	lua_Audio(const lua_Audio &);
	lua_Audio& operator=(const lua_Audio &);
};

#endif
