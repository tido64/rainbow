#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "lua.h"
#include "Physics.h"
#include "Lua/lua_Physics.h"

void lua_Physics::init(lua_State *L)
{
	lua_createtable(L, 0, 16);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, "physics");

	lua_pushcclosure(L, &lua_Physics::apply_force, 0);
	lua_setfield(L, -2, "apply_force");

	lua_pushcclosure(L, &lua_Physics::apply_angular_impulse, 0);
	lua_setfield(L, -2, "apply_angular_impulse");

	lua_pushcclosure(L, &lua_Physics::apply_torque, 0);
	lua_setfield(L, -2, "apply_torque");

	lua_pushcclosure(L, &lua_Physics::create_anchor, 0);
	lua_setfield(L, -2, "create_anchor");

	lua_pushcclosure(L, &lua_Physics::create_circle, 0);
	lua_setfield(L, -2, "create_circle");

	lua_pushcclosure(L, &lua_Physics::create_distance_joint, 0);
	lua_setfield(L, -2, "create_distance_joint");

	lua_pushcclosure(L, &lua_Physics::create_rope, 0);
	lua_setfield(L, -2, "create_rope");

	lua_pushcclosure(L, &lua_Physics::create_world, 0);
	lua_setfield(L, -2, "create_world");

	lua_pushcclosure(L, &lua_Physics::destroy, 0);
	lua_setfield(L, -2, "destroy");

	lua_pushcclosure(L, &lua_Physics::set_body_damping, 0);
	lua_setfield(L, -2, "set_body_damping");

	lua_pushcclosure(L, &lua_Physics::set_gravity, 0);
	lua_setfield(L, -2, "set_gravity");

	lua_pop(L, 1);
}

int lua_Physics::apply_force(lua_State *L)
{
	Physics::Instance().apply_force(lua_touserdata(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	return 0;
}

int lua_Physics::apply_angular_impulse(lua_State *L)
{
	Physics::Instance().apply_angular_impulse(lua_touserdata(L, 1), lua_tonumber(L, 2));
	return 0;
}

int lua_Physics::apply_torque(lua_State *L)
{
	Physics::Instance().apply_torque(lua_touserdata(L, 1), lua_tonumber(L, 2));
	return 0;
}

int lua_Physics::create_anchor(lua_State *L)
{
	lua_pushlightuserdata(L, Physics::Instance().create_anchor(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4)));
	return 1;
}

int lua_Physics::create_circle(lua_State *L)
{
	float friction = 0.2f;
	float restitution = 0.0f;
	float density = 0.0f;

	switch (lua_gettop(L))
	{
		case 6:
			density = lua_tonumber(L, 6);
		case 5:
			restitution = lua_tonumber(L, 5);
		case 4:
			friction = lua_tonumber(L, 4);
		default:
			break;
	}

	lua_pushlightuserdata(L, Physics::Instance().create_circle(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), friction, restitution, density));
	return 1;
}

int lua_Physics::create_distance_joint(lua_State *L)
{
	lua_pushlightuserdata(L, Physics::Instance().create_distance_joint(lua_touserdata(L, 1), lua_touserdata(L, 2)));
	return 1;
}

int lua_Physics::create_rope(lua_State *L)
{
	lua_pushlightuserdata(L, Physics::Instance().create_rope(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), (lua_gettop(L) == 5) ? lua_tonumber(L, 5) : 0));
	return 1;
}

int lua_Physics::create_world(lua_State *L)
{
	float g_x = 0.0f;
	float g_y = kStandardGravity;

	switch(lua_gettop(L))
	{
		case 2:
			g_y = lua_tonumber(L, 2);
		case 1:
			g_x = lua_tonumber(L, 1);
		default:
			break;
	}

	Physics::Instance().create_world(g_x, g_y);
	return 0;
}

int lua_Physics::destroy(lua_State *L)
{
	if (lua_gettop(L) < 1)
		Physics::Instance().destroy();
	else
		Physics::Instance().destroy(lua_touserdata(L, 1));
	return 0;
}

int lua_Physics::set_body_damping(lua_State *L)
{
	float linearDamping = 0.0f;
	float angularDamping = 0.1f;

	switch(lua_gettop(L))
	{
		case 2:
			angularDamping = lua_tonumber(L, 2);
		case 1:
			linearDamping = lua_tonumber(L, 1);
		default:
			break;
	}

	Physics::Instance().set_body_damping(lua_touserdata(L, 1), linearDamping, angularDamping);
	return 0;
}

int lua_Physics::set_gravity(lua_State *L)
{
	float g_x = 0.0f;
	float g_y = kStandardGravity;

	switch(lua_gettop(L))
	{
		case 2:
			g_y = lua_tonumber(L, 2);
		case 1:
			g_x = lua_tonumber(L, 1);
		default:
			break;
	}

	Physics::Instance().set_gravity(g_x, g_y);
	return 0;
}
