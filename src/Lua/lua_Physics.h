#ifndef LUA_PHYSICS_H_
#define LUA_PHYSICS_H_

#include "../Physics.h"
#include "../Lua.h"

class lua_Physics
{
public:
	static const char *class_name;
	static const Lua::Method<lua_Physics> methods[];

	lua_Physics(lua_State *) { }

	int create_anchor(lua_State *);
	int create_body(lua_State *);
	int create_joint(lua_State *);
	int define_body_damping(lua_State *);
	int define_body_position(lua_State *);
	int dispose_body(lua_State *);

private:
	Physics *p;
};

#endif
