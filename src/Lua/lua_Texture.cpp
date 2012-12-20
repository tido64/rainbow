#include "Common/Data.h"
#include "Lua/lua_Texture.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Texture::class_name[] = "texture";
		const Method<Texture> Texture::methods[] = {
			{ "create",  &Texture::create },
			{ "trim",    &Texture::trim },
			{ 0, 0 }
		};

		Texture::Texture(lua_State *L)
		{
			switch (lua_type(L, -1))
			{
				case LUA_TLIGHTUSERDATA:
					this->texture = static_cast<TextureAtlas*>(lua_touserdata(L, -1));
					break;
				case LUA_TSTRING:
					{
						Data tex_data(lua_tolstring(L, -1, nullptr));
						if (!tex_data)
							luaL_error(L, "rainbow.texture: Failed to load texture");
						this->texture = new TextureAtlas(tex_data);
						if (!*this->texture)
							luaL_error(L, "rainbow.texture: Failed to create texture");
					}
					break;
				default:
					LUA_ASSERT(lua_gettop(L) == 4, "rainbow.texture(<path to texture>)");
					break;
			}
		}

		int Texture::create(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 4, "<texture>:create(x, y, width, height)");

			const int x = lua_tointeger(L, 1);
			const int y = lua_tointeger(L, 2);
			const int w = lua_tointeger(L, 3);
			const int h = lua_tointeger(L, 4);
			lua_pushinteger(L, this->texture->define(x, y, w, h));
			return 1;
		}

		int Texture::trim(lua_State *)
		{
			this->texture->trim();
			return 0;
		}
	}
}
