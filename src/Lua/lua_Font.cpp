#include "Common/Data.h"
#include "Lua/lua_Font.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Font::class_name[] = "font";
		const LuaMachine::Method<Font> Font::methods[] = {
			{ 0, 0 }
		};

		Font::Font(lua_State *L) : font(new FontAtlas(lua_tonumber(L, 2)))
		{
			Data font(lua_tolstring(L, 1, nullptr));
			if (!font)
				luaL_error(L, "rainbow.font: Failed to load font");
			if (!this->font->load(font))
				luaL_error(L, "rainbow.font: Failed to create font texture");
		}
	}
}
