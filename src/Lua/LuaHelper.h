// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAHELPER_H_
#define LUA_LUAHELPER_H_

#include <new>

#include <lua.hpp>

#include "Lua/LuaMacros.h"

class Data;

NS_RAINBOW_LUA_BEGIN
{
	/// Creates a Lua wrapped object.
	template<class T>
	int alloc(lua_State *L);

	template<class T>
	int dealloc(lua_State *L);

	/// Outputs information about the error, and dumps the stack if applicable.
	void error(lua_State *L, const int lua_error);

	/// Custom Lua package loader.
	int load(lua_State *L);

	/// Loads buffer as a Lua chunk.
	/// \param chunk  Buffer to load.
	/// \param name   Name of the chunk. Used for debug information.
	/// \param exec   For internal use only! Whether to execute the loaded
	///               chunk. Only used by Lua package loaders.
	/// \return Number of successfully loaded chunks.
	int load(lua_State *L, const Data &chunk, const char *name, const bool exec = true);

	/// Returns the value returned from luaL_optinteger() but without the extra
	/// type check if NDEBUG is defined.
	inline lua_Integer optinteger(lua_State *L, const int n, lua_Integer def);

	/// Returns the value returned from luaL_optnumber() but without the extra
	/// type check if NDEBUG is defined.
	inline lua_Number optnumber(lua_State *L, const int n, lua_Number def);

	/// Pushes a collectable pointer on the stack.
	///
	/// Wraps pointer in a table so that one can attach an arbitrary metatable
	/// and have the garbage collector clean it up. Also sets the \c __type
	/// field for type checking.
	///
	/// \param ptr   The pointer to push on the stack.
	/// \param name  Name of the pointer type.
	void pushpointer(lua_State *L, void *ptr, const char *name);

	/// Simple C++-wrapper, loosely based on LunaWrapper.
	///
	/// Wraps a C++ object and makes its methods available in the namespace on
	/// top of the stack.
	///
	/// \see http://www.lua.org/manual/5.2/
	/// \see http://lua-users.org/wiki/LunaWrapper
	template<class T>
	void reg(lua_State *L);

	/// Reloads a previously loaded Lua chunk.
	/// \param chunk  Buffer to load.
	/// \param name   Name of the chunk. Used for debug information.
	/// \return Number of successfully reloaded chunks.
	int reload(lua_State *L, const Data &chunk, const char *name);

	/// Replaces the table at index \p n with its userdata if one exists.
	void replacetable(lua_State *L, const int n);

	/// Sets debugging hook.
	int sethook(lua_State *L, const int mask = LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE);

	/// Returns the value returned from luaL_checkinteger() or lua_tointeger(),
	/// depending on whether NDEBUG is defined.
	inline lua_Integer tointeger(lua_State *L, const int n);

	/// Returns the value returned from luaL_checknumber() or lua_tonumber(),
	/// depending on whether NDEBUG is defined.
	inline lua_Number tonumber(lua_State *L, const int n);

	/// Returns the pointer on top of the stack.
	///
	/// Unwraps the pointer while checking for nil value and type. This method
	/// may return a nullptr.
	///
	/// \see pushpointer()
	///
	/// \param name  Name of the pointer type to return.
	/// \return The pointer on the top of the stack if valid, else \c nullptr.
	void* topointer(lua_State *L, const char *name);

	/// Returns the string representing a Lua wrapped object. The format of the
	/// string is "<type name>: <address>".
	template<class T>
	int tostring(lua_State *L);

	/// Returns the pointer returned from luaL_checkudata() or lua_touserdata(),
	/// depending on whether NDEBUG is defined.
	template<class T>
	T* touserdata(lua_State *L, const int n);

	template<class T>
	int alloc(lua_State *L)
	{
		void *data = lua_newuserdata(L, sizeof(T));
		luaL_setmetatable(L, T::class_name);
		// Stash the userdata so we can return it later.
		const int ref = luaL_ref(L, LUA_REGISTRYINDEX);
		new (data) T(L);
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		luaL_unref(L, LUA_REGISTRYINDEX, ref);
		return 1;
	}

	template<class T>
	int dealloc(lua_State *L)
	{
		touserdata<T>(L, 1)->~T();
		return 0;
	}

	lua_Integer optinteger(lua_State *L, const int n, lua_Integer def)
	{
	#ifndef NDEBUG
		return luaL_optinteger(L, n, def);
	#else
		return luaL_opt(L, lua_tointeger, n, def);
	#endif
	}

	lua_Number optnumber(lua_State *L, const int n, lua_Number def)
	{
	#ifndef NDEBUG
		return luaL_optnumber(L, n, def);
	#else
		return luaL_opt(L, lua_tonumber, n, def);
	#endif
	}

	template<class T>
	void reg(lua_State *L)
	{
		if (T::is_constructible)
		{
			lua_pushstring(L, T::class_name);
			lua_pushcclosure(L, &alloc<T>, 0);
			lua_rawset(L, -3);
		}
		luaL_newmetatable(L, T::class_name);  // metatable = {}
		luaL_setfuncs(L, T::functions, 0);
		luaR_rawsetcclosurefield(L, &dealloc<T>, "__gc");
		luaR_rawsetcclosurefield(L, &tostring<T>, "__tostring");
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_rawset(L, -3);  // metatable.__index = metatable
		lua_pushliteral(L, "__metatable");
		lua_createtable(L, 0, 0);
		lua_rawset(L, -3);  // metatable.__metatable = {}
		lua_pop(L, 1);
	}

	lua_Integer tointeger(lua_State *L, const int n)
	{
	#ifndef NDEBUG
		return luaL_checkinteger(L, n);
	#else
		return lua_tointeger(L, n);
	#endif
	}

	lua_Number tonumber(lua_State *L, const int n)
	{
	#ifndef NDEBUG
		return luaL_checknumber(L, n);
	#else
		return lua_tonumber(L, n);
	#endif
	}

	template<class T>
	int tostring(lua_State *L)
	{
		lua_pushfstring(L, "%s: %p", T::class_name, touserdata<T>(L, 1));
		return 1;
	}

	template<class T>
	T* touserdata(lua_State *L, const int n)
	{
	#ifndef NDEBUG
		return static_cast<T*>(luaL_checkudata(L, n, T::class_name));
	#else
		return static_cast<T*>(lua_touserdata(L, n));
	#endif
	}
} NS_RAINBOW_LUA_END

#endif
