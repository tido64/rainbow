#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "Lua/lua_Sprite.h"

const char lua_Sprite::class_name[] = "_sprite";
const LuaMachine::Method<lua_Sprite> lua_Sprite::methods[] = {
	{ "get_angle",    &lua_Sprite::get_angle },
	{ "get_position", &lua_Sprite::get_position },
	{ "set_color",    &lua_Sprite::set_color },
	{ "set_pivot",    &lua_Sprite::set_pivot },
	{ "set_position", &lua_Sprite::set_position },
	{ "set_rotation", &lua_Sprite::set_rotation },
	{ "set_scale",    &lua_Sprite::set_scale },
	{ "set_texture",  &lua_Sprite::set_texture },
	{ 0, 0 }
};

lua_Sprite::lua_Sprite(lua_State *L)
{
	this->s = static_cast<Sprite *>(lua_touserdata(L, -1));
}

int lua_Sprite::get_angle(lua_State *L)
{
	lua_pushnumber(L, this->s->get_angle());
	return 1;
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
	this->s->set_color(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4));
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
	this->s->set_scale(lua_tonumber(L, 1));
	return 0;
}

int lua_Sprite::set_texture(lua_State *L)
{
	this->s->set_texture(lua_tointeger(L, 1));
	return 0;
}
