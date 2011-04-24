#include "lua_Font.h"

const char *lua_Font::class_name = "font";
const Lua::Method<lua_Font> lua_Font::methods[] = {
	{ "color", &lua_Font::color },
	{ "print", &lua_Font::print },
	{ 0 }
};

lua_Font::lua_Font(lua_State *L)  // const char *font_family, const float pt
{
	this->font = new FontAtlas(lua_tolstring(L, 1, 0), lua_tonumber(L, 2));
}

int lua_Font::color(lua_State *L)
{
	this->font->set_color(
		lua_tointeger(L, 1),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3),
		(lua_gettop(L) == 4) ? lua_tointeger(L, 4) : 0xff
	);
	return 0;
}

int lua_Font::print(lua_State *L)
{
	const int n = lua_gettop(L);
	const int x = (n >= 2) ? lua_tonumber(L, 2) : 0;
	const int y = (n == 3) ? lua_tonumber(L, 3) : 0;
	this->font->print(lua_tolstring(L, 1, 0), x, y);
	return 0;
}
