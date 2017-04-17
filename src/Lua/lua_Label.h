// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "Graphics/Label.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua
{
    class Label : private Object<Label>
    {
    public:
        LUA_REG_OBJECT_PROPS(true)

        explicit Label(lua_State*);

        auto get() { return &label_; }

    private:
        static auto get_color(lua_State*) -> int;
        static auto set_alignment(lua_State*) -> int;
        static auto set_color(lua_State*) -> int;
        static auto set_font(lua_State*) -> int;
        static auto set_position(lua_State*) -> int;
        static auto set_rotation(lua_State*) -> int;
        static auto set_scale(lua_State*) -> int;
        static auto set_text(lua_State*) -> int;
        static auto move(lua_State*) -> int;

        rainbow::Label label_;
    };
}}  // rainbow::lua

#endif
