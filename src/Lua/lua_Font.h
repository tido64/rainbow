#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "Graphics/FontAtlas.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	namespace Lua
	{
		class Font
		{
		public:
			static const char class_name[];
			static const Method<Font> methods[];

			Font(lua_State *);

			inline FontAtlas* raw_ptr() const;

		private:
			SharedPtr<FontAtlas> font;
		};

		FontAtlas* Font::raw_ptr() const
		{
			return this->font.raw_ptr();
		}
	}
}

#endif
