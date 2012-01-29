#ifndef LUA_RANDOM_H_
#define LUA_RANDOM_H_

struct lua_State;

class lua_Random
{
	friend class LuaMachine;

public:
	static int random(lua_State *);

private:
	static void init(lua_State *);

	lua_Random();
	lua_Random(const lua_Random &);
	lua_Random& operator=(const lua_Random &);
};

#endif
