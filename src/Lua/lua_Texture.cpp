#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "lua_Texture.h"

const char *lua_Texture::class_name = "load_texture";
const Lua::Method<lua_Texture> lua_Texture::methods[] = {
	{ "create_sprite",   &lua_Texture::create_sprite },
	{ "define_texture",  &lua_Texture::define_texture },
	{ "get_name",        &lua_Texture::get_name },
	{ 0 }
};

lua_Texture::lua_Texture(lua_State *L)
{
	this->t = new TextureAtlas(lua_tolstring(L, 1, 0), lua_tointeger(L, 2), lua_tointeger(L, 3), (lua_gettop(L) == 4) ? lua_tointeger(L, 4) : 0);
}

int lua_Texture::create_sprite(lua_State *L)
{
	lua_pushlightuserdata(L, this->t->create_sprite(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4)));
	return Lua::alloc<lua_Sprite>(L);
}

int lua_Texture::define_texture(lua_State *L)
{
	lua_pushinteger(L, this->t->define_texture(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4)));
	return 1;
}

int lua_Texture::get_name(lua_State *L)
{
	lua_pushinteger(L, this->t->get_name());
	return 1;
}
