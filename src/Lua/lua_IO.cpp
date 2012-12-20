#include <lua.hpp>

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Lua/lua_IO.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace IO
		{
			void init(lua_State *L)
			{
				lua_createtable(L, 0, 2);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "io");

				lua_pushcclosure(L, &load, 0);
				lua_setfield(L, -2, "load");

				lua_pushcclosure(L, &save, 0);
				lua_setfield(L, -2, "save");

				lua_pop(L, 1);
			}

			int load(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "rainbow.io.load(filename)");

				Data blob(lua_tolstring(L, 1, nullptr), 1);
				if (!blob)
					lua_pushnil(L);
				else
					lua_pushlstring(L, blob, blob.size());
				return 1;
			}

			int save(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 3, "rainbow.io.save(filename, data, size)");

				Data blob;
				blob.copy(lua_tolstring(L, 2, nullptr), lua_tointeger(L, 3));
				lua_pushboolean(L, blob.save(lua_tolstring(L, 1, nullptr)));
				return 1;
			}
		}
	}
}
