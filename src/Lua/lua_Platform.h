// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

#include "Lua/LuaMacros.h"
#include "Math/Vec2.h"

struct lua_State;

NS_RAINBOW_LUA_MODULE_BEGIN(platform)
{
    void init(lua_State* L);

    /// Updates rainbow.platform.screen.
    void update(lua_State* L, const Vec2i& screen);
} NS_RAINBOW_LUA_MODULE_END(platform)

#endif
