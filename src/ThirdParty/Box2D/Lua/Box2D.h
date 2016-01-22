// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_BOX2D_H_
#define THIRDPARTY_BOX2D_LUA_BOX2D_H_

struct lua_State;

/// Lua bindings for Box2D.
///
/// All classes, constants and functions are available under the 'b2' namespace
/// in place of their prefixes.
namespace b2
{
    namespace lua
    {
        void init(lua_State*);
    }
}

#endif
