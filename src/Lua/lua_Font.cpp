// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Common/Data.h"
#include "Lua/lua_Font.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Font::Bind::class_name[] = "font";

	template<>
	const bool Font::Bind::is_constructible = true;

	template<>
	const luaL_Reg Font::Bind::functions[] = { { 0, 0 } };

	Font::Font(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1) && lua_isnumber(L, 2),
		           "rainbow.font(\"path/to/fontface\", font_size)");

		const Data &font = Data::load_asset(lua_tostring(L, 1));
		if (!font)
			luaL_error(L, "rainbow.font: Failed to load font");
		this->font = new FontAtlas(font, lua_tonumber(L, 2));
		if (!*this->font)
			luaL_error(L, "rainbow.font: Failed to create font texture");
	}
} NS_RAINBOW_LUA_END
