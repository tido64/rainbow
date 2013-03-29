#include "Common/Data.h"
#include "Lua/lua_Font.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Font::class_name[] = "font";
		const Method<Font> Font::methods[] = {
			{ 0, 0 }
		};

		Font::Font(lua_State *L) : font(new FontAtlas(luaR_tonumber(L, 2)))
		{
			Data font(luaR_tostring(L, 1));
			if (!font)
				luaL_error(L, "rainbow.font: Failed to load font");
			if (!this->font->load(font))
				luaL_error(L, "rainbow.font: Failed to create font texture");
		}
	}
}
