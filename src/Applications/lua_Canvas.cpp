#ifdef USE_CANVAS
#include "Applications/lua_Canvas.h"
#include "Lua/lua_Texture.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Canvas::class_name[] = "canvas";
		const Method<Canvas> Canvas::methods[] = {
			{ "clear",          &Canvas::clear },
			{ "get_filled",     &Canvas::get_filled },
			{ "set_background", &Canvas::set_background },
			{ "set_brush",      &Canvas::set_brush },
			{ "set_brush_size", &Canvas::set_brush_size },
			{ "set_foreground", &Canvas::set_foreground },
			{ 0, 0 }
		};

		Canvas::~Canvas() { }

		int Canvas::get_filled(lua_State *L)
		{
			lua_pushnumber(L, ::Canvas::get_filled());
			return 1;
		}

		int Canvas::set_background(lua_State *L)
		{
			switch (lua_gettop(L))
			{
				case 1:
					::Canvas::set_background(lua_tointeger(L, 1));
					break;
				case 4:
					{
						const int i = lua_tointeger(L, 2);
						const int w = lua_tointeger(L, 3);
						const int h = lua_tointeger(L, 4);
						lua_pop(L, 3);
						Texture *texture = wrapper<Texture>(L);
						::Canvas::set_background((*texture->raw_ptr())[i], w, h);
					}
					break;
				default:
					LUA_ASSERT(false, "<canvas>:set_background(colour|texture_atlas, texture_id, width, height)");
					break;
			}
			return 0;
		}

		int Canvas::set_brush(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<canvas>:set_brush(texture_atlas, texture_id)");
			const int i = lua_tointeger(L, 2);
			lua_pop(L, 1);
			Texture *texture = wrapper<Texture>(L);
			::Canvas::set_brush((*texture->raw_ptr())[i]);
			return 0;
		}

		int Canvas::set_brush_size(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<canvas>:set_brush_size(diameter)");
			::Canvas::set_brush_size(lua_tointeger(L, 1));
			return 0;
		}

		int Canvas::set_foreground(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<canvas>:set_foreground(0xrrggbbaa)");
			::Canvas::set_foreground(lua_tointeger(L, 1));
			return 0;
		}
	}
}

#endif
