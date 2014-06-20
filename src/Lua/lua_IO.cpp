// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_IO.h"

#include "Common/Data.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

namespace
{
	int load(lua_State *L)
	{
		// rainbow.io.load(filename)
		Rainbow::Lua::Argument<char*>::is_required(L, 1);

		Data blob = Data::load_document(lua_tostring(L, 1));
		if (!blob)
			lua_pushnil(L);
		else
			lua_pushlstring(L, blob, blob.size());
		return 1;
	}

	int save(lua_State *L)
	{
		// rainbow.io.save(filename, data, size)
		Rainbow::Lua::Argument<char*>::is_required(L, 1);
		Rainbow::Lua::Argument<char*>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);

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
		luaR_rawsetcfunction(L, "load", &::load);
		luaR_rawsetcfunction(L, "save", &::save);
		lua_rawset(L, -3);
	}
} NS_RAINBOW_LUA_MODULE_END(IO)
