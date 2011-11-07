#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "LuaMachine.h"

struct lua_State;
class  Sprite;

class lua_Sprite
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Sprite> methods[];

	lua_Sprite(lua_State *);

	Sprite* raw_ptr() const;

	int get_angle(lua_State *);
	int get_position(lua_State *);
	int set_color(lua_State *);
	int set_pivot(lua_State *);
	int set_position(lua_State *);
	int set_rotation(lua_State *);
	int set_scale(lua_State *);
	int set_texture(lua_State *);

	int move(lua_State *);
	int rotate(lua_State *);
	int scale(lua_State *);

private:
	Sprite *s;
};

inline Sprite* lua_Sprite::raw_ptr() const
{
	return this->s;
}

#endif
