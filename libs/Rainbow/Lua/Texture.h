#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "../Lua.h"
//#include "../TextureAtlas.h"
#include "Sprite.h"

class lua_Texture
{
public:
	static const char *class_name;
	static const Lua::Method<lua_Texture> methods[];

	lua_Texture(lua_State *);
	~lua_Texture() { delete this->t; }

	int create_sprite(lua_State *);
	int define_texture(lua_State *);
	int get_name(lua_State *);

private:
	TextureAtlas *t;
};

#endif
