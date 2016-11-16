// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include <cstdlib>

#include "Lua/LuaMacros.h"
#include "Memory/Array.h"

class Acceleration;
struct Pointer;
struct lua_State;

namespace rainbow
{
    struct KeyStroke;
}

NS_RAINBOW_LUA_MODULE_BEGIN(input)
{
    void init(lua_State* L);

    void accelerated(lua_State* L, const Acceleration&);

    void clear(lua_State* L);

    void on_key_down(lua_State* L, const rainbow::KeyStroke& key);
    void on_key_up(lua_State* L, const rainbow::KeyStroke& key);

    void on_pointer_began(lua_State* L, const ArrayView<Pointer>& pointers);
    void on_pointer_canceled(lua_State* L);
    void on_pointer_ended(lua_State* L, const ArrayView<Pointer>& pointers);
    void on_pointer_moved(lua_State* L, const ArrayView<Pointer>& pointers);
}
NS_RAINBOW_LUA_MODULE_END(input)

#endif
