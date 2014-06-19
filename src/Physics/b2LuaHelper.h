// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

#define NS_B2_LUA_BEGIN namespace b2 { namespace Lua
#define NS_B2_LUA_END }

#define strllen(k) (sizeof(k) / sizeof(k[0]) - 1)

#define luaR_getboolean(L, k)  gettable<bool>(L, k, strllen(k))
#define luaR_getfield(L, k)    getfield(L, k, strllen(k))
#define luaR_getinteger(L, k)  gettable<lua_Integer>(L, k, strllen(k))
#define luaR_getnumber(L, k)   gettable<lua_Number>(L, k, strllen(k))
#define luaR_rawsetnil(L, k)   rawsetnil(L, k, strllen(k))
#define luaR_rawsetvec2(L, k)  rawsetvec2(L, k, strllen(k))

namespace
{
	void getfield(lua_State *L, const char *const field, const size_t length)
	{
		lua_pushlstring(L, field, length);
		lua_gettable(L, -2);
		LUA_ASSERT(
		    L, !lua_isnil(L, -1), "Missing field '%s' in definition", field);
	}

	template<typename T>
	T gettable(lua_State *L, const char *const field, const size_t length);

	template<>
	bool gettable<bool>(lua_State *L,
	                    const char *const field,
	                    const size_t length)
	{
		getfield(L, field, length);
		const bool b = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return b;
	}

	template<>
	lua_Integer gettable<lua_Integer>(lua_State *L,
	                                  const char *const field,
	                                  const size_t length)
	{
		getfield(L, field, length);
		const lua_Integer i = Rainbow::Lua::tointeger(L, -1);
		lua_pop(L, 1);
		return i;
	}

	template<>
	lua_Number gettable<lua_Number>(lua_State *L,
	                                const char *const field,
	                                const size_t length)
	{
		getfield(L, field, length);
		const lua_Number n = Rainbow::Lua::tonumber(L, -1);
		lua_pop(L, 1);
		return n;
	}

	void rawsetnil(lua_State *L, const char *const field, const size_t length)
	{
		lua_pushlstring(L, field, length);
		lua_pushnil(L);
		lua_rawset(L, -3);
	}

	int rawsetvec2(lua_State *L, const char *const field, const size_t length)
	{
		lua_pushlstring(L, field, length);
		lua_createtable(L, 0, 2);
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "x");
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "y");
		lua_rawset(L, -3);
		return 1;
	}
}
