#include "Applications/lua_Canvas.h"
#include "Common/RainbowAssert.h"
#include "Lua/lua_Texture.h"

const char lua_Canvas::class_name[] = "canvas";
const LuaMachine::Method<lua_Canvas> lua_Canvas::methods[] = {
	{ "clear",          &lua_Canvas::clear },
	{ "set_background", &lua_Canvas::set_background },
	{ "set_brush",      &lua_Canvas::set_brush },
	{ "set_brush_size", &lua_Canvas::set_brush_size },
	{ "set_foreground", &lua_Canvas::set_foreground },
	{ 0, 0 }
};

lua_Canvas::lua_Canvas(lua_State *) : canvas(new Canvas()) { }

lua_Canvas::~lua_Canvas()
{
	delete this->canvas;
}

int lua_Canvas::clear(lua_State *)
{
	this->canvas->clear();
	return 0;
}

int lua_Canvas::set_background(lua_State *L)
{
	switch (lua_gettop(L))
	{
		case 1:
			this->canvas->set_background(lua_tointeger(L, 1));
			break;
		case 4:
			{
				const int i = lua_tointeger(L, 2);
				const int w = lua_tointeger(L, 3);
				const int h = lua_tointeger(L, 4);
				lua_pop(L, 3);
				lua_Texture *texture = LuaMachine::wrapper<lua_Texture>(L);
				this->canvas->set_background((*texture->raw_ptr())[i], w, h);
			}
			break;
		default:
			LUA_ASSERT(false, "<canvas>:set_background(colour|texture_atlas, texture_id, width, height)");
			break;
	}
	return 0;
}

int lua_Canvas::set_brush(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 2, "<canvas>:set_brush(texture_atlas, texture_id)");
	const int i = lua_tointeger(L, 2);
	lua_pop(L, 1);
	lua_Texture *texture = LuaMachine::wrapper<lua_Texture>(L);
	this->canvas->set_brush((*texture->raw_ptr())[i]);
	return 0;
}

int lua_Canvas::set_brush_size(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "<canvas>:set_brush_size(diameter)");
	this->canvas->set_brush_size(lua_tointeger(L, 1));
	return 0;
}

int lua_Canvas::set_foreground(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "<canvas>:set_foreground(0xrrggbbaa)");
	this->canvas->set_foreground(lua_tointeger(L, 1));
	return 0;
}
