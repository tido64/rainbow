#include "Lua/LuaHelper.h"
#include "Lua/lua_Texture.h"

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<Texture, TextureAtlas, kBindTypeStrong> LuaTexture;

		template<>
		const char LuaTexture::class_name[] = "texture";

		template<>
		const Method<Texture> LuaTexture::methods[] = {
			{ "create",  &Texture::create },
			{ "trim",    &Texture::trim },
			{ 0, 0 }
		};

		Texture::Texture(lua_State *L)
		{
			switch (lua_type(L, -1))
			{
				case LUA_TLIGHTUSERDATA:
					this->ptr = static_cast<TextureAtlas*>(lua_touserdata(L, -1));
					break;
				case LUA_TSTRING: {
					DataMap data(luaR_tostring(L, -1));
					if (!data)
						luaL_error(L, "rainbow.texture: Failed to load texture");
					this->ptr = new TextureAtlas(data);
					if (!*this->ptr)
						luaL_error(L, "rainbow.texture: Failed to create texture");
					break;
				}
				default:
					LUA_ASSERT(lua_gettop(L) == 4, "rainbow.texture(<path to texture>)");
					break;
			}
		}

		int Texture::create(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 4, "<texture>:create(x, y, width, height)");

			const Vec2i origin(luaR_tointeger(L, 1), luaR_tointeger(L, 2));
			const int w = luaR_tointeger(L, 3);
			const int h = luaR_tointeger(L, 4);
			lua_pushinteger(L, this->ptr->define(origin, w, h));
			return 1;
		}

		int Texture::trim(lua_State *)
		{
			this->ptr->trim();
			return 0;
		}
	}
}
