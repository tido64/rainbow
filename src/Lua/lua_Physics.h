#ifndef LUA_PHYSICS_H_
#define LUA_PHYSICS_H_

struct lua_State;

class lua_Physics
{
public:
	static int apply_force(lua_State *);
	static int apply_angular_impulse(lua_State *);
	static int apply_torque(lua_State *);
	static int create_anchor(lua_State *);
	static int create_circle(lua_State *);
	static int create_distance_joint(lua_State *);
	static int create_rope(lua_State *);
	static int create_world(lua_State *);
	static int destroy(lua_State *);
	static int set_body_damping(lua_State *);
	static int set_gravity(lua_State *);

private:
	static void init(lua_State *);

	lua_Physics();
	lua_Physics(const lua_Physics &);
	lua_Physics& operator=(const lua_Physics &);

	friend class LuaMachine;
};

#endif
