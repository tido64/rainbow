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
	const Method<Font> Font::Bind::methods[] = {
		{ 0, 0 }
	};

	Font::Font(lua_State *L)
	{
		const Data &font = Data::load_asset(luaR_tostring(L, 1));
		if (!font)
			luaL_error(L, "rainbow.font: Failed to load font");
		this->ptr = new FontAtlas(font, luaR_tonumber(L, 2));
		if (!*this->ptr)
			luaL_error(L, "rainbow.font: Failed to create font texture");
	}
} NS_RAINBOW_LUA_END
