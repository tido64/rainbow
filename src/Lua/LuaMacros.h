// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAMACROS_H_
#define LUA_LUAMACROS_H_

#include "Common/Algorithm.h"
#include "Common/String.h"

#define NS_RAINBOW_LUA_BEGIN namespace rainbow { namespace lua
#define NS_RAINBOW_LUA_END }

#define NS_RAINBOW_LUA_MODULE_BEGIN(module) \
    namespace rainbow { namespace lua { namespace module
#define NS_RAINBOW_LUA_MODULE_END(module) }}

#define luaR_rawsetboolean(L, k, v) \
    rainbow::lua::rawset<bool>(L, k, rainbow::strllen(k), (v))

#define luaR_rawsetcfunction(L, k, v) \
    rainbow::lua::rawset<lua_CFunction>(L, k, rainbow::strllen(k), (v))

#define luaR_rawsetinteger(L, k, v) \
    rainbow::lua::rawset<lua_Integer>(L, k, rainbow::strllen(k), (v))

#define luaR_rawsetnumber(L, k, v) \
    rainbow::lua::rawset<lua_Number>(L, k, rainbow::strllen(k), (v))

#define luaR_rawsetstring(L, k, v) \
    rainbow::lua::rawset<czstring>(L, k, rainbow::strllen(k), (v))

#endif  // LUA_LUAMACROS_H_
