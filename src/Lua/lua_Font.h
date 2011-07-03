#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "Graphics/FontAtlas.h"
#include "LuaMachine.h"

class lua_Font
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Font> methods[];

	lua_Font(lua_State *);
	~lua_Font() { delete this->font; }

	int print(lua_State *);
	int set_color(lua_State *);

private:
	FontAtlas *font;
};

#endif
