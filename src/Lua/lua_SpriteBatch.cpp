#include "Common/DataMap.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"
#include "Lua/lua_Texture.h"

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<SpriteBatch, ::SpriteBatch, kBindTypeDerived> LuaSpriteBatch;

		template<>
		const char LuaSpriteBatch::class_name[] = "spritebatch";

		template<>
		const Method<SpriteBatch> LuaSpriteBatch::methods[] = {
			{ "add",            &SpriteBatch::add },
			{ "create_sprite",  &SpriteBatch::create_sprite },
			{ "set_texture",    &SpriteBatch::set_texture },
			{ 0, 0 }
		};

		SpriteBatch::SpriteBatch(lua_State *L) :
			::SpriteBatch(luaR_optinteger(L, 1, 8)) { }

		int SpriteBatch::add(lua_State *L)
		{
			R_DEBUG("[Rainbow] <spritebatch>:add() is deprecated, use <spritebatch>:create_sprite() and <sprite>:set_texture() instead.\n");
			LUA_ASSERT(lua_gettop(L) == 4, "<spritebatch>:add(x, y, width, height)");

			const int x = luaR_tointeger(L, 1);
			const int y = luaR_tointeger(L, 2);
			const int w = luaR_tointeger(L, 3);
			const int h = luaR_tointeger(L, 4);
			lua_pushlightuserdata(L, ::SpriteBatch::add(x, y, w, h));
			return alloc<Sprite>(L);
		}

		int SpriteBatch::create_sprite(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<spritebatch>:create_sprite(width, height)");

			const int w = luaR_tointeger(L, 1);
			const int h = luaR_tointeger(L, 2);
			lua_pushlightuserdata(L, ::SpriteBatch::create_sprite(w, h));
			return alloc<Sprite>(L);
		}

		int SpriteBatch::set_texture(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<spritebatch>:set_texture(<path to texture>|<texture>)");

			switch (lua_type(L, 1))
			{
				case LUA_TSTRING: {

						DataMap data(luaR_tostring(L, 1));
						if (!data)
							return luaL_error(L, "rainbow.spritebatch:set_texture: Failed to load texture");
						lua_pushlightuserdata(L, ::SpriteBatch::set_texture(data));
					return alloc<Texture>(L);
				}
				case LUA_TTABLE: {
						Texture *texture = wrapper<Texture>(L);
						::SpriteBatch::set_texture(texture->get());
					break;
				}
				default:
					LUA_ASSERT(false, "<spritebatch>:set_texture(<path to texture>|<texture>)");
					break;
			}
			return 0;
		}
	}
}
