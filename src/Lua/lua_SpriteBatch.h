// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITEBATCH_H_
#define LUA_SPRITEBATCH_H_

#include "Graphics/SpriteBatch.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua
{
    class SpriteBatch : private Object<SpriteBatch>
    {
    public:
        LUA_REG_OBJECT_PROPS(true)

        explicit SpriteBatch(lua_State*);

        auto get() { return &batch_; }

    private:
        static auto add(lua_State*) -> int;
        static auto create_sprite(lua_State*) -> int;
        static auto set_normal(lua_State*) -> int;
        static auto set_texture(lua_State*) -> int;

        rainbow::SpriteBatch batch_;
    };
}}  // rainbow::lua

#endif
