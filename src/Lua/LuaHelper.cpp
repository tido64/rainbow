#include <cstring>

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	namespace Lua
	{
		void dump_stack(lua_State *L)
		{
			R_DEBUG("Lua stack size: %i\n", lua_gettop(L));
			for (int l = 1; l <= lua_gettop(L); ++l)
			{
				R_DEBUG("#%i: ", l);
				switch (lua_type(L, l))
				{
					case LUA_TNIL:
						R_DEBUG("(nil)\n");
						break;
					case LUA_TNUMBER:
						R_DEBUG(LUA_NUMBER_FMT "\n", lua_tonumber(L, l));
						break;
					case LUA_TBOOLEAN:
						R_DEBUG("%s\n", lua_toboolean(L, l) ? "true" : "false");
						break;
					case LUA_TSTRING:
						R_DEBUG("%s\n", lua_tostring(L, l));
						break;
					case LUA_TTABLE:
						R_DEBUG("(table)\n");
						break;
					case LUA_TFUNCTION:
						R_DEBUG("%p\n", lua_topointer(L, l));
						break;
					case LUA_TUSERDATA:
						R_DEBUG("%p\n", lua_touserdata(L, l));
						break;
					case LUA_TTHREAD:
						R_DEBUG("(thread)\n");
						break;
					case LUA_TLIGHTUSERDATA:
						R_DEBUG("%p\n", lua_topointer(L, l));
						break;
					default:
						R_DEBUG("(unknown)\n");
						break;
				}
			}
		}

		void error(lua_State *L, const int lua_e)
		{
			R_ASSERT(lua_e != LUA_OK, "No error to report");

			const char err_general[] = "general";
			const char err_runtime[] = "runtime";
			const char err_syntax[]  = "syntax";
			const char err_memory[]  = "memory allocation";
			const char err_errfunc[] = "error handling";

			const char *desc = err_general;
			switch (lua_e)
			{
				case LUA_ERRRUN:
					desc = err_runtime;
					break;
				case LUA_ERRSYNTAX:
					desc = err_syntax;
					break;
				case LUA_ERRMEM:
					desc = err_memory;
					break;
				case LUA_ERRERR:
					desc = err_errfunc;
					break;
				default:
					break;
			}
			R_ERROR("Lua %s error: %s\n", desc, lua_tostring(L, -1));
			lua_pop(L, 1);
			dump_stack(L);
		}

		lua_Debug* getinfo(lua_State *L)
		{
			lua_Debug *d = new lua_Debug();
			lua_getstack(L, 1, d);
			lua_getinfo(L, "nSl", d);
			return d;
		}

		int load(lua_State *L)
		{
			const char *module = lua_tostring(L, -1);
			char *filename = new char[strlen(module) + 5];
			strcpy(filename, module);
			strcat(filename, ".lua");

			Data file(filename);
			delete[] filename;
			return (!file) ? 0 : load(L, file, module, false);
		}

		int load(lua_State *L, const Data &chunk, const char *name, const bool load)
		{
			int e = luaL_loadbuffer(L, chunk, chunk.size(), name);
			if (e || (load && (e = lua_pcall(L, 0, LUA_MULTRET, 0))))
			{
				error(L, e);
				return luaL_error(L, "Failed to load '%s'", name);
			}
			return 1;
		}

		void pushpointer(lua_State *L, void *ptr, const char *name)
		{
		#ifndef NDEBUG
			lua_createtable(L, 1, 1);
			lua_pushlightuserdata(L, ptr);
			lua_rawseti(L, -2, 0);
			luaR_rawsetfield(L, lua_pushstring, name, "__type");
		#else
			static_cast<void>(name);
			lua_pushlightuserdata(L, ptr);
		#endif
		}

		void* topointer(lua_State *L, const char *name)
		{
		#ifndef NDEBUG
			LUA_CHECK(L, !lua_isnil(L, -1), "Parameter is a nil value");
			LUA_CHECK(L, lua_istable(L, -1), "Object is not of type '%s'", name);
			lua_pushliteral(L, "__type");
			lua_rawget(L, -2);
			LUA_CHECK(L, memcmp(lua_tostring(L, -1), name, strlen(name)) == 0, "Object is not of type '%s'", name);
			lua_rawgeti(L, -2, 0);
			void *ptr = lua_touserdata(L, -1);
			lua_pop(L, 2);
			return ptr;
		#else
			static_cast<void>(name);
			return lua_touserdata(L, -1);
		#endif
		}
	}
}
