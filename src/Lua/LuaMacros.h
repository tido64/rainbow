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

#define luaR_rawgetfield(L, k, type) \
	do \
	{ \
		lua_pushliteral(L, k); \
		lua_rawget(L, -2); \
		LUA_ASSERT(L, !lua_isnil(L, -1), "%s is missing field '%s'", type, k); \
	} while(0)

#define luaR_rawsetcclosurefield(L, fn, k) \
	do \
	{ \
		lua_pushliteral(L, k); \
		lua_pushcclosure(L, fn, 0); \
		lua_rawset(L, -3); \
	} while(0)

#define luaR_rawsetfield(L, pushvalue, v, k) \
	do \
	{ \
		lua_pushliteral(L, k); \
		pushvalue(L, v); \
		lua_rawset(L, -3); \
	} while(0)

#define luaR_rawsetnilfield(L, k) \
	do \
	{ \
		lua_pushliteral(L, k); \
		lua_pushnil(L); \
		lua_rawset(L, -3); \
	} while(0)

#endif  // LUA_LUAMACROS_H_
