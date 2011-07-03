#ifndef LUA_SPRITEBATCH_H_
#define LUA_SPRITEBATCH_H_

#include "Graphics/SpriteBatch.h"
#include "LuaMachine.h"

class lua_SpriteBatch
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_SpriteBatch> methods[];

	lua_SpriteBatch(lua_State *L) : s(lua_tointeger(L, 1)) { }

	int add(lua_State *);
	int draw(lua_State *);
	int set_texture(lua_State *);
	int update(lua_State *);

private:
	SpriteBatch s;
};

#endif
