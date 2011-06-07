#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "../Lua.h"
#include "../Sprite.h"

class lua_Sprite
{
public:
	static const char *const class_name;
	static const Lua::Method<lua_Sprite> methods[];

	lua_Sprite(lua_State *);

	inline Sprite* raw_ptr() const { return this->s; }

	int get_angle(lua_State *);
	int get_position(lua_State *);
	int rotate(lua_State *);
	int scale(lua_State *);
	int set_color(lua_State *);
	int set_pivot(lua_State *);
	int set_position(lua_State *);
	int set_texture(lua_State *);

private:
	Sprite *s;
};

#endif
