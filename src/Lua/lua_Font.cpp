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
			Data font(luaR_tostring(L, 1));
			if (!font)
				luaL_error(L, "rainbow.font: Failed to load font");
			this->ptr = new FontAtlas(font, luaR_tonumber(L, 2));
			if (!*this->ptr)
				luaL_error(L, "rainbow.font: Failed to create font texture");
		}
	}
}
