#include "Lua/lua_Font.h"
#include "Lua/lua_Label.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Label::class_name[] = "label";
		const Method<Label> Label::methods[] = {
			{ "get_color",      &Label::get_color },
			{ "set_alignment",  &Label::set_alignment },
			{ "set_color",      &Label::set_color },
			{ "set_font",       &Label::set_font },
			{ "set_position",   &Label::set_position },
			{ "set_scale",      &Label::set_scale },
			{ "set_text",       &Label::set_text },
			{ "move",           &Label::move },
			{ 0, 0 }
		};

		Label::Label(lua_State *L)
		{
			if (lua_gettop(L))
				this->set_text(L);
		}

		int Label::get_color(lua_State *L)
		{
			const Colorb& c = ::Label::get_color();
			lua_pushinteger(L, c.r);
			lua_pushinteger(L, c.g);
			lua_pushinteger(L, c.b);
			lua_pushinteger(L, c.a);
			return 4;
		}

		int Label::set_alignment(lua_State *L)
		{
			switch (*lua_tolstring(L, 1, nullptr))
			{
				case 'c':
					::Label::set_alignment(Label::CENTER);
					break;
				case 'r':
					::Label::set_alignment(Label::RIGHT);
					break;
				default:
					::Label::set_alignment(Label::LEFT);
					break;
			}
			return 0;
		}

		int Label::set_color(lua_State *L)
		{
			const unsigned char r = lua_tointeger(L, 1);
			const unsigned char g = lua_tointeger(L, 2);
			const unsigned char b = lua_tointeger(L, 3);
			const unsigned char a = (lua_gettop(L) == 4) ? lua_tointeger(L, 4) : 0xff;
			::Label::set_color(Colorb(r, g, b, a));
			return 0;
		}

		int Label::set_font(lua_State *L)
		{
			Font *font = wrapper<Font>(L);
			::Label::set_font(font->raw_ptr());
			return 0;
		}

		int Label::set_position(lua_State *L)
		{
			const Vec2f position(lua_tonumber(L, 1), lua_tonumber(L, 2));
			::Label::set_position(position);
			return 0;
		}

		int Label::set_scale(lua_State *L)
		{
			::Label::set_scale(lua_tonumber(L, 1));
			return 0;
		}

		int Label::set_text(lua_State *L)
		{
			::Label::set_text(lua_tolstring(L, 1, nullptr));
			return 0;
		}

		int Label::move(lua_State *L)
		{
			const Vec2f delta(lua_tonumber(L, 1), lua_tonumber(L, 2));
			::Label::move(delta);
			return 0;
		}
	}
}
