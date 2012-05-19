#ifndef LUA_SPRITEBATCH_H_
#define LUA_SPRITEBATCH_H_

#include "LuaMachine.h"
#include "Graphics/SpriteBatch.h"

class lua_SpriteBatch : public SpriteBatch
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_SpriteBatch> methods[];

	inline lua_SpriteBatch(lua_State *L);

	inline SpriteBatch* raw_ptr();

	int add(lua_State *);
	int set_texture(lua_State *);
};

lua_SpriteBatch::lua_SpriteBatch(lua_State *L) :
	SpriteBatch(lua_tointeger(L, 1)) { }

SpriteBatch* lua_SpriteBatch::raw_ptr()
{
	return this;
}

#endif
