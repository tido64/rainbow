#ifndef LUAMACHINE_H_
#define LUAMACHINE_H_

#include <cstring>

#include <lualib.h>
#include <lauxlib.h>

namespace SceneGraph { class Node; }
class lua_SceneGraph;

/// Embeds Lua scripting engine.
///
/// Features a simple C++-wrapper, adapted from lua-users.org
///
/// \see http://www.lua.org/manual/5.2/
/// \see http://lua-users.org/wiki/LunaWrapper
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class LuaMachine
{
	friend class Director;

public:
	template<class T>
	struct Method
	{
		const char *const name;
		int (T::*lua_CFunction)(lua_State *);
	};

	/// Create a Lua wrapped object.
	template<class T>
    static int alloc(lua_State *L);

	/// Dump Lua stack to stdout.
	static void dump_stack(lua_State *L);

	/// Return information about a specific function or function invocation.
	static lua_Debug* getinfo(lua_State *L);

	template<class T>
	static int thunk(lua_State *L);

	/// Wrap a C++ object and make its methods available in the namespace on top
	/// of the stack.
	template<class T>
	static void wrap(lua_State *L);

	/// Return the wrapper of the object on top of the stack.
	/// \return Pointer to wrapper.
	template<class T>
	static T* wrapper(lua_State *L, const int index = -1);

	~LuaMachine();

	/// Call game update function.
	int update(const unsigned long t);

private:
	template<class T>
	static int dealloc(lua_State *L);

	lua_SceneGraph *scenegraph;
	lua_State *L;

	LuaMachine();

	/// Intentionally left undefined.
	LuaMachine(const LuaMachine &);

	/// Call a function with no parameters or return value.
	/// \param k         Name of the function to call.
	/// \param nargs     Number of arguments pushed to the stack.
	/// \param nresults  Number of results to push to the stack; LUA_MULTRET for all.
	/// \return Lua error code (defined in lua.h).
	int call(const char *const, int nargs = 0, int nresults = 0);

	/// Output information about the error, and dump the stack if applicable.
	void err(const int lua_error);

	/// Load specified Lua script.
	/// \return Lua error code (defined in lua.h).
	int load(SceneGraph::Node *root, const char *const);

	/// Intentionally left undefined.
	LuaMachine& operator=(const LuaMachine &);
};

template<class T>
int LuaMachine::alloc(lua_State *L)
{
	T *obj = new T(L);
	lua_pop(L, lua_gettop(L));

	lua_createtable(L, 0, 0);
	lua_pushnumber(L, 0);

	T **ptr = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
	*ptr = obj;

	lua_getfield(L, LUA_REGISTRYINDEX, T::class_name);
	lua_setmetatable(L, -2);
	lua_settable(L, -3);

	for (int i = 0; T::methods[i].name; ++i)
	{
		lua_pushstring(L, T::methods[i].name);
		lua_pushnumber(L, i);
		lua_pushcclosure(L, &LuaMachine::thunk<T>, 1);
		lua_settable(L, -3);
	}

	return 1;
}

template<class T>
int LuaMachine::dealloc(lua_State *L)
{
	T **ptr = static_cast<T **>(luaL_checkudata(L, -1, T::class_name));
	delete *ptr;
	return 0;
}

template<class T>
int LuaMachine::thunk(lua_State *L)
{
#ifndef NDEBUG

	if (lua_type(L, 1) != LUA_TTABLE)
		return luaL_error(L, "Called a class function using '.' instead of ':'");

#endif

	const int i = static_cast<int>(lua_tonumber(L, lua_upvalueindex(1)));
	lua_rawgeti(L, 1, 0);
	lua_remove(L, 1);

	T **ptr = static_cast<T**>(luaL_checkudata(L, -1, T::class_name));
	lua_pop(L, 1);

	return ((*ptr)->*(T::methods[i].lua_CFunction))(L);
}

template<class T>
void LuaMachine::wrap(lua_State *L)
{
	lua_pushcclosure(L, &LuaMachine::alloc<T>, 0);
	lua_setfield(L, -2, T::class_name);
	luaL_newmetatable(L, T::class_name);
	lua_pushstring(L, "__gc");
	lua_pushcclosure(L, &LuaMachine::dealloc<T>, 0);
	lua_settable(L, -3);
	lua_pop(L, 1);
}

template<class T>
T* LuaMachine::wrapper(lua_State *L, const int index)
{
	// Get user data from table
	lua_rawgeti(L, index, 0);
	void *ptr = luaL_checkudata(L, -1, T::class_name);
	lua_pop(L, 1);
	return *static_cast<T**>(ptr);
}

#endif
