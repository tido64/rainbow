#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "LuaMachine.h"

class Animation;

class lua_Animation
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Animation> methods[];

	lua_Animation(lua_State *);
	~lua_Animation();
	inline Animation* raw_ptr() const;

	int is_stopped(lua_State *);

	int set_delay(lua_State *);
	int set_fps(lua_State *);
	int set_frames(lua_State *);

	int play(lua_State *);
	int stop(lua_State *);

private:
	Animation *animation;
};

Animation* lua_Animation::raw_ptr() const
{
	return this->animation;
}

#endif
