// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "Graphics/Sprite.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua
{
    class Sprite : private Object<Sprite>
    {
    public:
        LUA_REG_OBJECT_PROPS(false)

        explicit Sprite(lua_State*);

        auto get() -> SpriteRef& { return sprite_; }

    private:
        static auto get_angle(lua_State*) -> int;
        static auto get_color(lua_State*) -> int;
        static auto get_pivot(lua_State*) -> int;
        static auto get_position(lua_State*) -> int;
        static auto get_scale(lua_State*) -> int;
        static auto get_size(lua_State*) -> int;
        static auto set_color(lua_State*) -> int;
        static auto set_normal(lua_State*) -> int;
        static auto set_pivot(lua_State*) -> int;
        static auto set_position(lua_State*) -> int;
        static auto set_rotation(lua_State*) -> int;
        static auto set_scale(lua_State*) -> int;
        static auto set_texture(lua_State*) -> int;

        static auto mirror(lua_State*) -> int;
        static auto move(lua_State*) -> int;
        static auto rotate(lua_State*) -> int;

        SpriteRef sprite_;
    };
}}  // rainbow::lua

#endif
