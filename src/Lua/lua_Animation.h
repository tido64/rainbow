// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "Graphics/Animation.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua
{
    class Animation : private Object<Animation>
    {
    public:
        LUA_REG_OBJECT_PROPS(true)

        explicit Animation(lua_State*);

        auto get() { return animation_.get(); }

    private:
        static auto current_frame(lua_State*) -> int;
        static auto is_stopped(lua_State*) -> int;
        static auto set_delay(lua_State*) -> int;
        static auto set_frame_rate(lua_State*) -> int;
        static auto set_frames(lua_State*) -> int;
        static auto set_listener(lua_State*) -> int;
        static auto set_sprite(lua_State*) -> int;
        static auto jump_to(lua_State*) -> int;
        static auto rewind(lua_State*) -> int;
        static auto start(lua_State*) -> int;
        static auto stop(lua_State*) -> int;

        std::unique_ptr<rainbow::Animation> animation_;
        WeakRef listener_;
    };
}}  // rainbow::lua

#endif
