// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Font.h"

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<Font, FontAtlas, kBindTypeStrong> LuaFont;

		template<>
		const char LuaFont::class_name[] = "font";

		template<>
		const Method<Font> LuaFont::methods[] = {
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
	}
}
