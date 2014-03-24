// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "FileSystem/Path.h"
#include "Lua/lua_Texture.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Texture::Bind::class_name[] = "texture";

	template<>
	const Method<Texture> Texture::Bind::methods[] = {
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
				DataMap data(Path(luaR_tostring(L, -1)));
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
} NS_RAINBOW_LUA_END
