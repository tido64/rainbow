// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Label.h"

#include "Lua/LuaHelper.h"
#include "Lua/lua_Font.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Label::Bind::class_name[] = "label";

	template<>
	const bool Label::Bind::is_constructible = true;

	template<>
	const luaL_Reg Label::Bind::functions[] = {
		{ "get_color",      &Label::get_color },
		{ "set_alignment",  &Label::set_alignment },
		{ "set_color",      &Label::set_color },
		{ "set_font",       &Label::set_font },
		{ "set_position",   &Label::set_position },
		{ "set_scale",      &Label::set_scale },
		{ "set_text",       &Label::set_text },
		{ "move",           &Label::move },
		{ nullptr, nullptr }
	};

	Label::Label(lua_State *L)
	{
		if (lua_isstring(L, 1))
			this->label.set_text(lua_tostring(L, 1));
	}

	int Label::get_color(lua_State *L)
	{
		Label *self = Bind::self(L);
		if (!self)
			return 0;

		const Colorb& c = self->label.color();
		lua_pushinteger(L, c.r);
		lua_pushinteger(L, c.g);
		lua_pushinteger(L, c.b);
		lua_pushinteger(L, c.a);
		return 4;
	}

	int Label::set_alignment(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 2),
		           "<label>:set_alignment('l' | 'c' | 'r')");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		switch (*lua_tostring(L, 2))
		{
			case 'c':
				self->label.set_alignment(::Label::kCenterTextAlignment);
				break;
			case 'r':
				self->label.set_alignment(::Label::kRightTextAlignment);
				break;
			default:
				self->label.set_alignment(::Label::kLeftTextAlignment);
				break;
		}
		return 0;
	}

	int Label::set_color(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) &&
		           lua_isnumber(L, 3) &&
		           lua_isnumber(L, 4) &&
		           (lua_isnumber(L, 5) || lua_isnone(L, 5)),
		           "<label>:set_color(r, g, b, a = 255)");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		const unsigned char r = lua_tointeger(L, 2);
		const unsigned char g = lua_tointeger(L, 3);
		const unsigned char b = lua_tointeger(L, 4);
		const unsigned char a = luaR_optinteger(L, 5, 0xff);
		self->label.set_color(Colorb(r, g, b, a));
		return 0;
	}

	int Label::set_font(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "<label>:set_font(<font>)");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		self->label.set_font(touserdata<Font>(L, 2)->get());
		return 0;
	}

	int Label::set_position(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) && lua_isnumber(L, 3),
		           "<label>:set_position(x, y)");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		self->label.set_position(
		    Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}

	int Label::set_scale(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2), "<label>:set_scale(f)");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		self->label.set_scale(lua_tonumber(L, 2));
		return 0;
	}

	int Label::set_text(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 2), "<label>:set_text(\"string\")");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		self->label.set_text(lua_tostring(L, 2));
		return 0;
	}

	int Label::move(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) && lua_isnumber(L, 3),
		           "<label>:move(x, y)");

		Label *self = Bind::self(L);
		if (!self)
			return 0;

		self->label.move(Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}
} NS_RAINBOW_LUA_END
