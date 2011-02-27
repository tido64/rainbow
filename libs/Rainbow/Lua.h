#ifndef LUA_H_
#define LUA_H_

#include <cassert>
#include <cstdlib>
#include <cstring>

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}


/// Embeds Lua scripting engine.

/// Features a simple C++-wrapper, adapted from lua-users.org
///
/// \see http://www.lua.org/manual/5.1/
/// \see http://lua-users.org/wiki/LunaWrapper
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Lua
{
public:
	template<class T>
	struct Method
	{
		const char *name;
		int (T::*lua_CFunction)(lua_State *);
	};

	Lua();
	~Lua() { lua_close(this->L); }

	/// Call a function with no parameters or return value
	/// \param k  Name of the function to call
	void call(const char *k);

	/// Load given Lua script
	void load(const char *);

	/// Wraps a C++ object and makes its methods available in Lua
	/// \param ns  Inserts object into given namespace
	template<class T>
	void wrap(const char *ns = 0)
	{
		if (ns != 0 && strcmp(ns, "") != 0)
		{
			lua_getfield(this->L, LUA_GLOBALSINDEX, ns);

			if (!lua_istable(this->L, -1))
			{
				lua_pop(this->L, 1);
				lua_newtable(this->L);
				lua_pushvalue(this->L, -1);
				lua_setfield(L, LUA_GLOBALSINDEX, ns);
			}

			lua_pushcclosure(this->L, &Lua::alloc<T>, 0);
			lua_setfield(this->L, -2, T::class_name);
			lua_pop(this->L, 1);
		}
		else
		{
			lua_pushcclosure(this->L, &Lua::alloc<T>, 0);
			lua_setfield(this->L, LUA_GLOBALSINDEX, T::class_name);
		}

		luaL_newmetatable(this->L, T::class_name);
		const int meta_tbl = lua_gettop(this->L);

		lua_pushstring(this->L, "__gc");
		lua_pushcclosure(this->L, &Lua::dealloc<T>, 0);
		lua_settable(this->L, meta_tbl);
	}

private:
	lua_State *L;

	template<class T>
    static int alloc(lua_State * L)
	{
		T *obj = new T(L);

		lua_newtable(L);
		lua_pushnumber(L, 0);
		T **ptr = static_cast<T **>(lua_newuserdata(L, sizeof(T*)));
		*ptr = obj;

		luaL_getmetatable(L, T::class_name);
		lua_setmetatable(L, -2);
		lua_settable(L, -3);

		for (int i = 0; T::methods[i].name; ++i)
		{
			lua_pushstring(L, T::methods[i].name);
			lua_pushnumber(L, i);
			lua_pushcclosure(L, &Lua::thunk<T>, 1);
			lua_settable(L, -3);
		}

		return 1;
    }

	template<class T>
	static int dealloc(lua_State *L)
	{
		T **ptr = static_cast<T **>(luaL_checkudata(L, -1, T::class_name));
		delete *ptr;
		return 0;
	}

	template<class T>
	static int thunk(lua_State *L)
	{
		const int i = static_cast<int>(lua_tonumber(L, lua_upvalueindex(1)));
		lua_pushnumber(L, 0);
		lua_gettable(L, 1);

		T **ptr = static_cast<T **>(luaL_checkudata(L, -1, T::class_name));
		lua_remove(L, -1);

		return ((*ptr)->*(T::methods[i].lua_CFunction))(L);
	}
};

#include "Lua/Algorithm.h"
#include "Lua/Font.h"
#include "Lua/Physics.h"
#include "Lua/Platform.h"
#include "Lua/Sprite.h"
#include "Lua/Texture.h"

#endif
