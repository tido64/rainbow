// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Lua/lua_IO.h"

namespace
{
	int load(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1), "rainbow.io.load(filename)");

		Data blob = Data::load_document(lua_tostring(L, 1));
		if (!blob)
			lua_pushnil(L);
		else
			lua_pushlstring(L, blob, blob.size());
		return 1;
	}

	int save(lua_State *L)
	{
		LUA_ASSERT(lua_isstring(L, 1) &&
		           lua_isstring(L, 2) &&
		           lua_isnumber(L, 3),
		           "rainbow.io.save(filename, data, size)");

		Data blob(
		    lua_tostring(L, 2), lua_tointeger(L, 3), Data::kDataReference);
		lua_pushboolean(L, blob.save(lua_tostring(L, 1)));
		return 1;
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(IO)
{
	void init(lua_State *L)
	{
		lua_pushliteral(L, "io");
		lua_createtable(L, 0, 2);
		luaR_rawsetcclosurefield(L, &load, "load");
		luaR_rawsetcclosurefield(L, &save, "save");
		lua_rawset(L, -3);
	}
} NS_RAINBOW_LUA_MODULE_END(IO)
