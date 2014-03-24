// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaHelper.h"
#include "Lua/lua_Font.h"
#include "Lua/lua_Label.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Label::Bind::class_name[] = "label";

	template<>
	const Method<Label> Label::Bind::methods[] = {
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
		const Colorb& c = ::Label::color();
		lua_pushinteger(L, c.r);
		lua_pushinteger(L, c.g);
		lua_pushinteger(L, c.b);
		lua_pushinteger(L, c.a);
		return 4;
	}

	int Label::set_alignment(lua_State *L)
	{
		switch (*luaR_tostring(L, 1))
		{
			case 'c':
				::Label::set_alignment(::Label::kCenterTextAlignment);
				break;
			case 'r':
				::Label::set_alignment(::Label::kRightTextAlignment);
				break;
			default:
				::Label::set_alignment(::Label::kLeftTextAlignment);
				break;
		}
		return 0;
	}

	int Label::set_color(lua_State *L)
	{
		const unsigned char r = luaR_tointeger(L, 1);
		const unsigned char g = luaR_tointeger(L, 2);
		const unsigned char b = luaR_tointeger(L, 3);
		const unsigned char a = luaR_optinteger(L, 4, 0xff);
		::Label::set_color(Colorb(r, g, b, a));
		return 0;
	}

	int Label::set_font(lua_State *L)
	{
		Font *font = wrapper<Font>(L);
		::Label::set_font(font->get());
		return 0;
	}

	int Label::set_position(lua_State *L)
	{
		const Vec2f position(luaR_tonumber(L, 1), luaR_tonumber(L, 2));
		::Label::set_position(position);
		return 0;
	}

	int Label::set_scale(lua_State *L)
	{
		::Label::set_scale(luaR_tonumber(L, 1));
		return 0;
	}

	int Label::set_text(lua_State *L)
	{
		::Label::set_text(luaR_tostring(L, 1));
		return 0;
	}

	int Label::move(lua_State *L)
	{
		const Vec2f delta(luaR_tonumber(L, 1), luaR_tonumber(L, 2));
		::Label::move(delta);
		return 0;
	}
} NS_RAINBOW_LUA_END
