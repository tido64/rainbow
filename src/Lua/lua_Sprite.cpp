#include "Common/RainbowAssert.h"
#include "Graphics/Sprite.h"
#include "Lua/lua_Sprite.h"

const char lua_Sprite::class_name[] = "_sprite";
const LuaMachine::Method<lua_Sprite> lua_Sprite::methods[] = {
	{ "get_angle",    &lua_Sprite::get_angle },
	{ "get_color",    &lua_Sprite::get_color },
	{ "get_position", &lua_Sprite::get_position },
	{ "set_color",    &lua_Sprite::set_color },
	{ "set_pivot",    &lua_Sprite::set_pivot },
	{ "set_position", &lua_Sprite::set_position },
	{ "set_rotation", &lua_Sprite::set_rotation },
	{ "set_scale",    &lua_Sprite::set_scale },
	{ "set_texture",  &lua_Sprite::set_texture },
	{ "mirror",       &lua_Sprite::mirror },
	{ "move",         &lua_Sprite::move },
	{ "rotate",       &lua_Sprite::rotate },
	{ 0, 0 }
};

lua_Sprite::lua_Sprite(lua_State *L) :
	s(static_cast<Sprite*>(lua_touserdata(L, -1))) { }

int lua_Sprite::get_angle(lua_State *L)
{
	lua_pushnumber(L, this->s->get_angle());
	return 1;
}

int lua_Sprite::get_color(lua_State *L)
{
	const Colorb& c = this->s->get_color();
	lua_pushinteger(L, c.r);
	lua_pushinteger(L, c.g);
	lua_pushinteger(L, c.b);
	lua_pushinteger(L, c.a);
	return 4;
}

int lua_Sprite::get_position(lua_State *L)
{
	const Vec2f &v = this->s->get_position();
	lua_pushnumber(L, v.x);
	lua_pushnumber(L, v.y);
	return 2;
}

int lua_Sprite::set_color(lua_State *L)
{
	unsigned int color = lua_tointeger(L, 1) << 24;
	color += lua_tointeger(L, 2) << 16;
	color += lua_tointeger(L, 3) << 8;
	color += lua_tointeger(L, 4);
	this->s->set_color(color);
	return 0;
}

int lua_Sprite::set_pivot(lua_State *L)
{
	this->s->set_pivot(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 0;
}

int lua_Sprite::set_position(lua_State *L)
{
	this->s->set_position(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 0;
}

int lua_Sprite::set_rotation(lua_State *L)
{
	this->s->set_rotation(lua_tonumber(L, 1));
	return 0;
}

int lua_Sprite::set_scale(lua_State *L)
{
	switch (lua_gettop(L))
	{
		case 2:
			this->s->set_scale(Vec2f(lua_tonumber(L, 1), lua_tonumber(L, 2)));
			break;
		case 1:
			this->s->set_scale(lua_tonumber(L, 1));
			break;
		default:
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "<sprite>:set_scale(fx [, fy])");
			break;
	}
	return 0;
}

int lua_Sprite::set_texture(lua_State *L)
{
	this->s->set_texture(lua_tointeger(L, 1));
	return 0;
}

int lua_Sprite::mirror(lua_State *)
{
	this->s->mirror();
	return 0;
}

int lua_Sprite::move(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 2, "<sprite>:move(x, y)");

	this->s->move(lua_tonumber(L, 1), lua_tonumber(L, 2));
	return 0;
}

int lua_Sprite::rotate(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "<sprite>:rotate(r)");

	this->s->rotate(lua_tonumber(L, 1));
	return 0;
}
