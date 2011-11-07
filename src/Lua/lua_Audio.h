#ifndef LUA_AUDIO_H_
#define LUA_AUDIO_H_

struct lua_State;

class lua_Audio
{
public:
	static int set_doppler_factor(lua_State *);
	static int set_doppler_velocity(lua_State *);
	static int set_speed_of_sound(lua_State *);

	// Listener
	static int set_gain(lua_State *);
	static int set_orientation(lua_State *);

	// Listener + source
	static int set_pitch(lua_State *);
	static int set_position(lua_State *);
	static int set_velocity(lua_State *);

	// Source
	static int add(lua_State *);
	static int play(lua_State *);
	static int set_looping(lua_State *);

	// BGM
	static int set_bgm(lua_State *);
	static int play_bgm(lua_State *);
	static int pause_bgm(lua_State *);
	static int stop_bgm(lua_State *);
	static int set_bgm_pan(lua_State *);
	static int set_bgm_volume(lua_State *);

private:
	static void init(lua_State *);

	lua_Audio();
	lua_Audio(const lua_Audio &);
	lua_Audio& operator=(const lua_Audio &);

	friend class LuaMachine;
};

#endif
