#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "../Lua.h"
#include "../Texture.h"

class lua_Texture
{
public:
	static const char *const class_name;
	static const Lua::Method<lua_Texture> methods[];

	lua_Texture(lua_State *);

	int create(lua_State *);
	int trim(lua_State *);

private:
	Texture *t;
};

#endif
