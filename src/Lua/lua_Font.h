#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "LuaMachine.h"

class FontAtlas;

class lua_Font
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Font> methods[];

	lua_Font(lua_State *);
	~lua_Font();

	int print(lua_State *);
	int set_color(lua_State *);

private:
	FontAtlas *font;
};

#endif
