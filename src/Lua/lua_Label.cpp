#include "Lua/lua_Font.h"
#include "Lua/lua_Label.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Label::class_name[] = "label";
		const LuaMachine::Method<Label> Label::methods[] = {
			{ "set_alignment",  &Label::set_alignment },
			{ "set_color",      &Label::set_color },
			{ "set_font",       &Label::set_font },
			{ "set_position",   &Label::set_position },
			{ "set_text",       &Label::set_text },
			{ 0, 0 }
		};

		Label::Label(lua_State *L)
		{
			if (lua_gettop(L))
				this->set_text(L);
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
			Font *font = LuaMachine::wrapper<Font>(L);
			::Label::set_font(font->raw_ptr());
			return 0;
		}

		int Label::set_position(lua_State *L)
		{
			const int x = static_cast<int>(lua_tonumber(L, 1) + 0.5f);
			const int y = static_cast<int>(lua_tonumber(L, 2) + 0.5f);
			::Label::set_position(x, y);
			return 0;
		}

		int Label::set_text(lua_State *L)
		{
			::Label::set_text(lua_tolstring(L, 1, nullptr));
			return 0;
		}
	}
}
