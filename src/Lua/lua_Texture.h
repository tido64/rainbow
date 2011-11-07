#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "LuaMachine.h"

class Texture;

class lua_Texture
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Texture> methods[];

	lua_Texture(lua_State *);

	int create(lua_State *);
	int trim(lua_State *);

private:
	Texture *t;
};

#endif
