// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_IO.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace IO
		{
			namespace
			{
				int load(lua_State *L)
				{
					LUA_ASSERT(lua_gettop(L) == 1, "rainbow.io.load(filename)");

					Data blob = Data::load_document(luaR_tostring(L, 1));
					if (!blob)
						lua_pushnil(L);
					else
						lua_pushlstring(L, blob, blob.size());
					return 1;
				}

				int save(lua_State *L)
				{
					LUA_ASSERT(lua_gettop(L) == 3, "rainbow.io.save(filename, data, size)");

					Data blob(luaR_tostring(L, 2), luaR_tointeger(L, 3), Data::kDataReference);
					lua_pushboolean(L, blob.save(luaR_tostring(L, 1)));
					return 1;
				}
			}

			void init(lua_State *L)
			{
				lua_pushliteral(L, "io");
				lua_createtable(L, 0, 2);
				luaR_rawsetcclosurefield(L, &load, "load");
				luaR_rawsetcclosurefield(L, &save, "save");
				lua_rawset(L, -3);
			}
		}
	}
}
