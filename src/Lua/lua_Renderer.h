// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_RENDERER_H_
#define LUA_RENDERER_H_

#include "Lua/LuaMacros.h"

struct lua_State;

NS_RAINBOW_LUA_MODULE_BEGIN(renderer)
{
    void init(lua_State*);
} NS_RAINBOW_LUA_MODULE_END(renderer)

#endif
