#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "Common/Data.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"
#include "Lua/lua_Texture.h"

const char lua_SpriteBatch::class_name[] = "spritebatch";
const LuaMachine::Method<lua_SpriteBatch> lua_SpriteBatch::methods[] = {
	{ "add",          &lua_SpriteBatch::add },
	{ "draw",         &lua_SpriteBatch::draw },
	{ "set_texture",  &lua_SpriteBatch::set_texture },
	{ "update",       &lua_SpriteBatch::update },
	{ 0, 0 }
};

int lua_SpriteBatch::add(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 4, "<spritebatch>:add(x, y, width, height)");

	const int x = lua_tointeger(L, 1);
	const int y = lua_tointeger(L, 2);
	const int w = lua_tointeger(L, 3);
	const int h = lua_tointeger(L, 4);
	lua_pushlightuserdata(L, this->s.add(x, y, w, h));
	return LuaMachine::alloc<lua_Sprite>(L);
}

int lua_SpriteBatch::draw(lua_State *)
{
	this->s.draw();
	return 0;
}

int lua_SpriteBatch::set_texture(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "<spritebatch>:set_texture(<path to texture>|<texture>)");

	switch (lua_type(L, 1))
	{
		case LUA_TSTRING:
			{
				const char *const path = Data::get_path(lua_tolstring(L, 1, nullptr));
				Data t(path);
				Data::free(path);
				if (!t)
					return luaL_error(L, "rainbow.spritebatch:set_texture: Failed to load texture");
				lua_pushlightuserdata(L, this->s.set_texture(t));
			}
			return LuaMachine::alloc<lua_Texture>(L);
		case LUA_TTABLE:
			{
				lua_Texture *texture = LuaMachine::wrapper<lua_Texture>(L);
				this->s.set_texture(texture->raw_ptr());
			}
			break;
		default:
			LUA_ASSERT(false, "<spritebatch>:set_texture(<path to texture>|<texture>)");
			break;
	}
	return 0;
}

int lua_SpriteBatch::update(lua_State *)
{
	this->s.update();
	return 0;
}
