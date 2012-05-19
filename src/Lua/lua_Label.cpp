#include "Lua/lua_Font.h"
#include "Lua/lua_Label.h"

const char lua_Label::class_name[] = "label";
const LuaMachine::Method<lua_Label> lua_Label::methods[] = {
	{ "set_color",     &lua_Label::set_color },
	{ "set_font",      &lua_Label::set_font },
	{ "set_position",  &lua_Label::set_position },
	{ "set_text",      &lua_Label::set_text },
	{ 0, 0 }
};

lua_Label::lua_Label(lua_State *L)
{
	if (lua_gettop(L))
		this->set_text(L);
}

int lua_Label::set_color(lua_State *L)
{
	const unsigned char r = lua_tointeger(L, 1);
	const unsigned char g = lua_tointeger(L, 2);
	const unsigned char b = lua_tointeger(L, 3);
	const unsigned char a = (lua_gettop(L) == 4) ? lua_tointeger(L, 4) : 0xff;
	Label::set_color(Colorb(r, g, b, a));
	return 0;
}

int lua_Label::set_font(lua_State *L)
{
	lua_Font *font = LuaMachine::wrapper<lua_Font>(L);
	Label::set_font(font->raw_ptr());
	return 0;
}

int lua_Label::set_position(lua_State *L)
{
	const int x = static_cast<int>(lua_tonumber(L, 1) + 0.5f);
	const int y = static_cast<int>(lua_tonumber(L, 2) + 0.5f);
	Label::set_position(x, y);
	return 0;
}

int lua_Label::set_text(lua_State *L)
{
	Label::set_text(lua_tolstring(L, 1, nullptr));
	return 0;
}
