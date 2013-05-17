#ifndef LUA_HELPER_H_
#define LUA_HELPER_H_

#include <lua.hpp>

#include "Lua/LuaBind.h"

#define luaR_rawgetfield(L, k, type) \
	lua_pushliteral(L, k); \
	lua_rawget(L, -2); \
	LUA_CHECK(L, !lua_isnil(L, -1), "%s is missing field '%s'", type, k)

#define luaR_rawsetcclosurefield(L, fn, n, k) \
	lua_pushliteral(L, k); \
	lua_pushcclosure(L, fn, n); \
	lua_rawset(L, -3)

#define luaR_rawsetfield(L, pushvalue, v, k) \
	lua_pushliteral(L, k); \
	pushvalue(L, v); \
	lua_rawset(L, -3)

#define luaR_rawsetnilfield(L, k) \
	lua_pushliteral(L, k); \
	lua_pushnil(L); \
	lua_rawset(L, -3)

#ifndef NDEBUG
#	define luaR_optinteger(L, narg, def)    luaL_optinteger(L, narg, def)
#	define luaR_tointeger(L, narg)          luaL_checkinteger(L, narg)
#	define luaR_tonumber(L, narg)           luaL_checknumber(L, narg)
#	define luaR_tostring(L, narg)           luaL_checkstring(L, narg)
#	define luaR_touserdata(L, narg, tname)  luaL_checkudata(L, narg, tname)
#else
#	define luaR_optinteger(L, narg, def)    luaL_opt(L, lua_tointeger, narg, def)
#	define luaR_tointeger(L, narg)          lua_tointeger(L, narg)
#	define luaR_tonumber(L, narg)           lua_tonumber(L, narg)
#	define luaR_tostring(L, narg)           lua_tostring(L, narg)
#	define luaR_touserdata(L, narg, tname)  lua_touserdata(L, narg)
#endif

class Data;

namespace Rainbow
{
	namespace Lua
	{
		/// Create a Lua wrapped object.
		template<class T>
		int alloc(lua_State *L)
		{
			T *obj = new T(L);

			lua_createtable(L, 0, 0);
		#ifndef NDEBUG
			lua_pushliteral(L, "__type");
			lua_pushstring(L, T::class_name);
			lua_rawset(L, -3);
		#endif
			lua_pushnumber(L, 0);

			T **ptr = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
			*ptr = obj;

			lua_getfield(L, LUA_REGISTRYINDEX, T::class_name);
			lua_rawgeti(L, -1, 1);
			lua_setmetatable(L, -5);
			lua_setmetatable(L, -2);
			lua_rawset(L, -3);

			return 1;
		}

		template<class T>
		int dealloc(lua_State *L)
		{
			T **ptr = static_cast<T**>(luaR_touserdata(L, -1, T::class_name));
			delete *ptr;
			return 0;
		}

		/// Dump Lua stack to stdout.
		void dump_stack(lua_State *L);

		/// Output information about the error, and dump the stack if applicable.
		void error(lua_State *L, const int lua_error);

		/// Return information about a specific function or function invocation.
		lua_Debug* getinfo(lua_State *L);

		/// Custom Lua package loader.
		int load(lua_State *L);

		/// Load buffer as a Lua chunk.
		/// \param chunk  Buffer to load.
		/// \param name   Name of the chunk. Used for debug information.
		/// \param load   For internal use only! Whether to execute the loaded
		///               chunk. Only used by Lua package loaders.
		/// \return See \c luaL_loadbuffer().
		int load(lua_State *L, const Data &chunk, const char *name, const bool load = true);

		/// Push a pointer on the stack.
		///
		/// Equivalent to calling \c lua_pushlightuserdata() but if \c NDEBUG is
		/// not defined, attaches a \c __type field to the pointer for easier
		/// debugging.
		///
		/// \param ptr   The pointer to push on the stack.
		/// \param name  Name of the pointer type.
		void pushpointer(lua_State *L, void *ptr, const char *name);

		/// Call a function in a wrapped object. The first parameter passed must
		/// be the \c self object.
		template<class T>
		int thunk(lua_State *L)
		{
		#ifndef NDEBUG
			if (lua_type(L, 1) != LUA_TTABLE)
				return luaL_error(L, "Called a class function using '.' instead of ':'");
		#endif

			const int i = static_cast<int>(lua_tonumber(L, lua_upvalueindex(1)));
			lua_rawgeti(L, 1, 0);
			lua_remove(L, 1);

			T **ptr = static_cast<T**>(luaR_touserdata(L, -1, T::class_name));
			lua_pop(L, 1);

			return ((*ptr)->*(T::methods[i].lua_CFunction))(L);
		}

		/// Return the pointer on top of the stack.
		///
		/// Equivalent to calling \c lua_touserdata() but if \c NDEBUG is not
		/// defined, ensures that \p name matches the pointer's \c __type field
		/// before returning it.
		///
		/// \see pushpointer()
		///
		/// \param name  Name of the pointer type to return.
		/// \return The pointer on the top of the stack if valid, else \c nullptr.
		void* topointer(lua_State *L, const char *name);

		/// Simple C++-wrapper, adapted from lua-users.org.
		///
		/// Wrap a C++ object and make its methods available in the namespace on
		/// top of the stack.
		///
		/// \see http://www.lua.org/manual/5.2/
		/// \see http://lua-users.org/wiki/LunaWrapper
		template<class T>
		void wrap(lua_State *L, const bool internal = false)
		{
			if (!internal)
			{
				lua_pushstring(L, T::class_name);
				lua_pushcclosure(L, &alloc<T>, 0);
				lua_rawset(L, -3);
			}
			luaL_newmetatable(L, T::class_name);
			lua_pushnumber(L, 1);
			lua_createtable(L, 0, 1);
			lua_pushliteral(L, "__index");
			lua_createtable(L, 0, 0);
			for (int i = 0; T::methods[i].name; ++i)
			{
				lua_pushstring(L, T::methods[i].name);
				lua_pushnumber(L, i);
				lua_pushcclosure(L, &thunk<T>, 1);
				lua_rawset(L, -3);
			}
			lua_rawset(L, -3);
			lua_rawset(L, -3);
			luaR_rawsetcclosurefield(L, &dealloc<T>, 0, "__gc");
			lua_pop(L, 1);
		}

		/// Return the wrapper of the object on top of the stack.
		/// \return Pointer to wrapper.
		template<class T>
		T* wrapper(lua_State *L, const int index = -1)
		{
			// Get user data from table.
			lua_rawgeti(L, index, 0);
			void *ptr = luaR_touserdata(L, -1, T::class_name);
			lua_pop(L, 1);
			return *static_cast<T**>(ptr);
		}
	}
}

#endif
