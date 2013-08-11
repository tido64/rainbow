#include <cstring>

#include "Common/Data.h"
#include "Lua/LuaDebugging.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace
		{
			const char kLuaErrorGeneral[] = "general";
			const char kLuaErrorRuntime[] = "runtime";
			const char kLuaErrorSyntax[]  = "syntax";
			const char kLuaErrorMemory[]  = "memory allocation";
			const char kLuaErrorErrorHandling[] = "error handling";
			const char kLuaErrorType[] = "Object is not of type '%s'";
		}

		void error(lua_State *L, const int lua_e)
		{
			R_ASSERT(lua_e != LUA_OK, "No error to report");

			const char *desc = kLuaErrorGeneral;
			switch (lua_e)
			{
				case LUA_ERRRUN:
					desc = kLuaErrorRuntime;
					break;
				case LUA_ERRSYNTAX:
					desc = kLuaErrorSyntax;
					break;
				case LUA_ERRMEM:
					desc = kLuaErrorMemory;
					break;
				case LUA_ERRERR:
					desc = kLuaErrorErrorHandling;
					break;
				default:
					break;
			}
			R_ERROR("* Lua %s error: %s\n", desc, lua_tostring(L, -1));
			lua_pop(L, 1);
			dump_stack(L);
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
			lua_createtable(L, 1, 1);
			lua_pushlightuserdata(L, ptr);
			lua_rawseti(L, -2, 0);
			luaR_rawsetfield(L, lua_pushstring, name, "__type");
		}

		int sethook(lua_State *L, const int mask)
		{
			if (g_level >= 0)
				return 0;

			int depth = -1;
			lua_Debug ar;
			while (lua_getstack(L, ++depth, &ar));
			--depth;
			while (lua_getstack(L, ++g_level, &ar))
			{
				lua_getinfo(L, "Sl", &ar);
				g_callstack[depth - g_level].currentline = ar.currentline;
				g_callstack[depth - g_level].nparams = ar.nparams;
				g_callstack[depth - g_level].source = ar.source;
			}

			return lua_sethook(L, lua_Hook, mask, 0);
		}

		void* topointer(lua_State *L, const char *name)
		{
			LUA_CHECK(L, !lua_isnil(L, -1), "Parameter is a nil value");
			LUA_CHECK(L, lua_istable(L, -1), kLuaErrorType, name);
			lua_pushliteral(L, "__type");
			lua_rawget(L, -2);
			const char *type = lua_tostring(L, -1);
			if (!type)
			{
				LUA_CHECK(L, type, kLuaErrorType, name);
				lua_pop(L, 1);
				return nullptr;
			}
			LUA_CHECK(L, strcmp(type, name) == 0, kLuaErrorType, name);
			lua_rawgeti(L, -2, 0);
			void *ptr = lua_touserdata(L, -1);
			lua_pop(L, 2);
			return ptr;
			static_cast<void>(name);
		}
	}
}
