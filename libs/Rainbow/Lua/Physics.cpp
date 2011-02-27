#include "Physics.h"

const char *lua_Physics::class_name = "physics";
const Lua::Method<lua_Physics> lua_Physics::methods[] = {
	{ "create_anchor",        &lua_Physics::create_anchor },
	{ "create_body",          &lua_Physics::create_body },
	{ "create_joint",         &lua_Physics::create_joint },
	{ "define_body_damping",  &lua_Physics::define_body_damping },
	{ "define_body_position", &lua_Physics::define_body_position },
	{ "dispose_body",         &lua_Physics::dispose_body },
	{ 0 }
};

int lua_Physics::create_anchor(lua_State *L)  // const float &w, const float &h, const float &x, const float &y
{
	Body *b = Physics::Instance().create_anchor(
		lua_tointeger(L, 1),
		lua_tointeger(L, 2),
		lua_tointeger(L, 3),
		lua_tointeger(L, 4));
	lua_pushlightuserdata(L, b);
	return 1;  // b2Body *
}

int lua_Physics::create_body(lua_State *L)  // const b2BodyDef *d, const b2FixtureDef *fixture = 0, const float inertia = 0.0f, const float mass = 0.0f
{
	Body *b = 0;
	//Physics::Instance().create_body();
	lua_pushlightuserdata(L, b);
	return 1;  // b2Body *
}

int lua_Physics::create_joint(lua_State *L)  // b2Body *a, b2Body *b, const b2Vec2 &a_pos, const b2Vec2 &b_pos
{
	Joint *j = 0;
	//Physics::Instance().create_body();
	lua_pushlightuserdata(L, j);
	return 1;  // b2Joint *
}

int lua_Physics::define_body_damping(lua_State *L)  // b2BodyDef *d, const float linearDamping = 0.0f, const float angularDamping = 0.1f
{
	//Physics::Instance().define_body_damping();
	return 0;
}

int lua_Physics::define_body_position(lua_State *L)  // b2BodyDef *d, const float x = 0.0f, const float y = 0.0f
{
	//Physics::Instance().define_body_position();
	return 0;
}

int lua_Physics::dispose_body(lua_State *L)  // b2Body *b
{
	//Physics::Instance().dispose_body();
	return 0;
}
