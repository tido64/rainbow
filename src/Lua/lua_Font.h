#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "../FontAtlas.h"
#include "../Lua.h"

class lua_Font
{
public:
	static const char *const class_name;
	static const Lua::Method<lua_Font> methods[];

	lua_Font(lua_State *);
	~lua_Font() { delete this->font; }

	int print(lua_State *);
	int set_color(lua_State *);

private:
	FontAtlas *font;
};

#endif
