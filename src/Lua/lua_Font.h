#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "LuaMachine.h"
#include "Graphics/FontAtlas.h"

class lua_Font
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Font> methods[];

	lua_Font(lua_State *);

	inline FontAtlas* raw_ptr() const;

private:
	SmartPtr<FontAtlas> font;
};

FontAtlas* lua_Font::raw_ptr() const
{
	return this->font.raw_ptr();
}

#endif
