#include "Common/Data.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"
#include "Lua/lua_Texture.h"

namespace Rainbow
{
	namespace Lua
	{
		const char SpriteBatch::class_name[] = "spritebatch";
		const LuaMachine::Method<SpriteBatch> SpriteBatch::methods[] = {
			{ "add",          &SpriteBatch::add },
			{ "set_texture",  &SpriteBatch::set_texture },
			{ 0, 0 }
		};

		int SpriteBatch::add(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 4, "<spritebatch>:add(x, y, width, height)");

			const int x = lua_tointeger(L, 1);
			const int y = lua_tointeger(L, 2);
			const int w = lua_tointeger(L, 3);
			const int h = lua_tointeger(L, 4);
			lua_pushlightuserdata(L, ::SpriteBatch::add(x, y, w, h));
			return LuaMachine::alloc<Sprite>(L);
		}

		int SpriteBatch::set_texture(lua_State *L)
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
						lua_pushlightuserdata(L, ::SpriteBatch::set_texture(t));
					}
					return LuaMachine::alloc<Texture>(L);
				case LUA_TTABLE:
					{
						Texture *texture = LuaMachine::wrapper<Texture>(L);
						::SpriteBatch::set_texture(texture->raw_ptr());
					}
					break;
				default:
					LUA_ASSERT(false, "<spritebatch>:set_texture(<path to texture>|<texture>)");
					break;
			}
			return 0;
		}
	}
}
