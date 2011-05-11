#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "lua_SpriteBatch.h"

const char *lua_SpriteBatch::class_name = "spritebatch";
const Lua::Method<lua_SpriteBatch> lua_SpriteBatch::methods[] = {
	{ "add",     &lua_SpriteBatch::add },
	{ "draw",    &lua_SpriteBatch::draw },
	{ "update",  &lua_SpriteBatch::update },
	{ 0 }
};

int lua_SpriteBatch::add(lua_State *L)
{
	this->s.add(Lua::wrapper<lua_Sprite>(L)->raw_ptr());
	return 0;
}

int lua_SpriteBatch::draw(lua_State *L)
{
	this->s.draw();
	return 0;
}

int lua_SpriteBatch::update(lua_State *L)
{
	this->s.update();
	return 0;
}
