#include "Texture.h"

const char *lua_Texture::class_name = "texture";
const Lua::Method<lua_Texture> lua_Texture::methods[] = {
	{ "create_sprite",  &lua_Texture::create_sprite },
	{ "define_texture", &lua_Texture::define_texture },
	{ "get_name",       &lua_Texture::get_name },
	{ 0 }
};

lua_Texture::lua_Texture(lua_State *L)
{
	// const char *filename, const int texture_count, const int sprite_count, const int mipmap = 0
	const int mipmap = (lua_gettop(L) == 4) ? lua_tointeger(L, 4) : 0;
	this->t = new TextureAtlas(
		lua_tolstring(L, 1, 0),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3),
		mipmap);
}

int lua_Texture::create_sprite(lua_State *L)
{
	// const int x, const int y, const int width, const int height
	Sprite *s = this->t->create_sprite(
		lua_tointeger(L, 1),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3),
		lua_tointeger(L, 4));
	lua_pushlightuserdata(L, s);
	Lua::dump_stack(L);
	return 1;
}

int lua_Texture::define_texture(lua_State *L)
{
	// const int x, const int y, const int width, const int height
	unsigned int name = this->t->define_texture(
		lua_tointeger(L, 1),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3),
		lua_tointeger(L, 4));
	lua_pushinteger(L, name);
	return 1;
}

int lua_Texture::get_name(lua_State *L)
{
	lua_pushinteger(L, this->t->get_name());
	return 1;
}
