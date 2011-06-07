#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "lua_SpriteBatch.h"

const char *const lua_SpriteBatch::class_name = "spritebatch";
const Lua::Method<lua_SpriteBatch> lua_SpriteBatch::methods[] = {
	{ "add",          &lua_SpriteBatch::add },
	{ "draw",         &lua_SpriteBatch::draw },
	{ "set_texture",  &lua_SpriteBatch::set_texture },
	{ "update",       &lua_SpriteBatch::update },
	{ 0 }
};

int lua_SpriteBatch::add(lua_State *L)
{
	assert(lua_gettop(L) == 4 || !"Rainbow::Lua::SpriteBatch::add takes four parameters");
	lua_pushlightuserdata(L, this->s.add(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4)));
	return Lua::alloc<lua_Sprite>(L);
}

int lua_SpriteBatch::draw(lua_State *L)
{
	this->s.draw();
	return 0;
}

int lua_SpriteBatch::set_texture(lua_State *L)
{
	assert(lua_gettop(L) == 1 || !"Rainbow::Lua::SpriteBatch::add takes only one parameter");
	lua_pushlightuserdata(L, this->s.set_texture(lua_tolstring(L, 1, 0)));
	return Lua::alloc<lua_Texture>(L);
}

int lua_SpriteBatch::update(lua_State *L)
{
	this->s.update();
	return 0;
}
