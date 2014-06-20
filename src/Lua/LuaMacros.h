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

#define strllen(k) (sizeof(k) / sizeof(k[0]) - 1)

#define luaR_rawsetboolean(L, k, v) \
	Rainbow::Lua::rawset<bool>(L, k, strllen(k), (v))

#define luaR_rawsetcfunction(L, k, v) \
	Rainbow::Lua::rawset<lua_CFunction>(L, k, strllen(k), (v))

#define luaR_rawsetinteger(L, k, v) \
	Rainbow::Lua::rawset<lua_Integer>(L, k, strllen(k), (v))

#define luaR_rawsetnumber(L, k, v) \
	Rainbow::Lua::rawset<lua_Number>(L, k, strllen(k), (v))

#define luaR_rawsetstring(L, k, v) \
	Rainbow::Lua::rawset<const char*>(L, k, strllen(k), (v))

#endif  // LUA_LUAMACROS_H_
