// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Texture.h"

#include <lua.hpp>

#include "FileSystem/Path.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Texture::Bind::class_name[] = "texture";

	template<>
	const bool Texture::Bind::is_constructible = true;

	template<>
	const luaL_Reg Texture::Bind::functions[] = {
		{ "create",  &Texture::create },
		{ "trim",    &Texture::trim },
		{ nullptr, nullptr }
	};

	Texture::Texture(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1), "rainbow.texture(\"/path/to/texture\")");

		DataMap data(Path(lua_tostring(L, 1)));
		if (!data)
			luaL_error(L, "rainbow.texture: Failed to load texture");
		this->texture = new TextureAtlas(data);
		if (!*this->texture)
			luaL_error(L, "rainbow.texture: Failed to create texture");
	}

	int Texture::create(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) &&
		           lua_isnumber(L, 3) &&
		           lua_isnumber(L, 4) &&
		           lua_isnumber(L, 5),
		           "<texture>:create(x, y, width, height)");

		Texture *self = Bind::self(L);
		if (!self)
			return 0;

		const Vec2i origin(lua_tointeger(L, 2), lua_tointeger(L, 3));
		const int w = lua_tointeger(L, 4);
		const int h = lua_tointeger(L, 5);
		lua_pushinteger(L, self->texture->define(origin, w, h));
		return 1;
	}

	int Texture::trim(lua_State *L)
	{
		Texture *self = Bind::self(L);
		if (!self)
			return 0;

		self->texture->trim();
		return 0;
	}
} NS_RAINBOW_LUA_END
