#include "Common/Data.h"
#include "Common/Debug.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_IO.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace IO
		{
			void init(lua_State *L)
			{
				lua_pushliteral(L, "io");
				lua_createtable(L, 0, 2);
				lua_rawsetcclosurefield(L, &load, 0, "load");
				lua_rawsetcclosurefield(L, &save, 0, "save");
				lua_rawset(L, -3);
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
