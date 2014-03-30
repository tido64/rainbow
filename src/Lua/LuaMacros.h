// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAMACROS_H_
#define LUA_LUAMACROS_H_

#define NS_RAINBOW_LUA_BEGIN namespace Rainbow { namespace Lua
#define NS_RAINBOW_LUA_END }

#define NS_RAINBOW_LUA_MODULE_BEGIN(module) \
	namespace Rainbow { namespace Lua { namespace module
#define NS_RAINBOW_LUA_MODULE_END(module) }}

#define luaR_isuserdata(L, n) (lua_isuserdata(L, n) || lua_istable(L, n))

#define luaR_rawgetfield(L, k, type) \
	lua_pushliteral(L, k); \
	lua_rawget(L, -2); \
	LUA_CHECK(L, !lua_isnil(L, -1), "%s is missing field '%s'", type, k)

#define luaR_rawsetcclosurefield(L, fn, k) \
	lua_pushliteral(L, k); \
	lua_pushcclosure(L, fn, 0); \
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
#	define luaR_optnumber(L, narg, def)     luaL_optnumber(L, narg, def)
#	define luaR_tointeger(L, narg)          luaL_checkinteger(L, narg)
#	define luaR_tonumber(L, narg)           luaL_checknumber(L, narg)
#	define luaR_tostring(L, narg)           luaL_checkstring(L, narg)
#	define luaR_touserdata(L, narg, tname)  luaL_checkudata(L, narg, tname)
#else
#	define luaR_optinteger(L, narg, def)    luaL_opt(L, lua_tointeger, narg, def)
#	define luaR_optnumber(L, narg, def)     luaL_opt(L, lua_tonumber, narg, def)
#	define luaR_tointeger(L, narg)          lua_tointeger(L, narg)
#	define luaR_tonumber(L, narg)           lua_tonumber(L, narg)
#	define luaR_tostring(L, narg)           lua_tostring(L, narg)
#	define luaR_touserdata(L, narg, tname)  lua_touserdata(L, narg)
#endif  // NDEBUG

#endif  // LUA_LUAMACROS_H_
