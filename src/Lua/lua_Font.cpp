// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Font.h"

#include <lua.hpp>

#include "Common/Data.h"

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
		// rainbow.font("path/to/fontface", font_size)
		Argument<char*>::is_required(L, 1);
		Argument<lua_Number>::is_required(L, 2);

		const Data &font = Data::load_asset(lua_tostring(L, 1));
		if (!font)
			luaL_error(L, "rainbow.font: Failed to load font");
		font_.reset(new FontAtlas(font, lua_tonumber(L, 2)));
		if (!font_->is_valid())
			luaL_error(L, "rainbow.font: Failed to create font texture");
	}

	SharedPtr<FontAtlas> Font::get() const
	{
		return font_;
	}
} NS_RAINBOW_LUA_END
