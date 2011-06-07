#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "lua_Font.h"

const char *const lua_Font::class_name = "font";
const Lua::Method<lua_Font> lua_Font::methods[] = {
	{ "print", &lua_Font::print },
	{ "set_color", &lua_Font::set_color },
	{ 0 }
};

lua_Font::lua_Font(lua_State *L)
{
	this->font = new FontAtlas(lua_tolstring(L, 1, 0), lua_tonumber(L, 2));
}

int lua_Font::print(lua_State *L)
{
	int x = 0;
	int y = 0;

	switch (lua_gettop(L))
	{
		case 3:
			y = static_cast<int>(lua_tonumber(L, 3) + 0.5f);
		case 2:
			x = static_cast<int>(lua_tonumber(L, 2) + 0.5f);
		default:
			break;
	}

	this->font->print(lua_tolstring(L, 1, 0), x, y);
	return 0;
}

int lua_Font::set_color(lua_State *L)
{
	this->font->set_color(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), (lua_gettop(L) == 4) ? lua_tointeger(L, 4) : 0xff);
	return 0;
}
