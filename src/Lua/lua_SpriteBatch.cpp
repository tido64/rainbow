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
	assert(lua_gettop(L) == 4 || !"Rainbow::Lua::SpriteBatch::add takes four parameters");

	lua_pushlightuserdata(L, this->s.add(lua_tointeger(L, 1),
	                                     lua_tointeger(L, 2),
	                                     lua_tointeger(L, 3),
	                                     lua_tointeger(L, 4)));
	return LuaMachine::alloc<lua_Sprite>(L);
}

int lua_SpriteBatch::draw(lua_State *)
{
	this->s.draw();
	return 0;
}

int lua_SpriteBatch::set_texture(lua_State *L)
{
	assert(lua_gettop(L) == 1 || !"Rainbow::Lua::SpriteBatch::set_texture takes only one parameter");

	switch (lua_type(L, 1))
	{
		case LUA_TSTRING:
			{
				const char *const path = Data::get_path(lua_tolstring(L, 1, nullptr));
				Data t(path);
				Data::free(path);
				lua_pushlightuserdata(L, this->s.set_texture(t));
				return LuaMachine::alloc<lua_Texture>(L);
			}
			break;
		case LUA_TLIGHTUSERDATA:
			//lua_pushlightuserdata(L, this->s.set_texture(reinterpret_cast<Texture *>(lua_topointer(L, 1))));
			break;
		default:
			break;
	}
	return 0;
}

int lua_SpriteBatch::update(lua_State *)
{
	this->s.update();
	return 0;
}
