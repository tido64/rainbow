// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "Graphics/Animation.h"
#include "Lua/LuaBind.h"

class Animation;

NS_RAINBOW_LUA_BEGIN
{
    class Animation : private Bind<Animation>
    {
    public:
        static constexpr bool is_constructible = true;
        static const char class_name[];
        static const luaL_Reg functions[];

        Animation(lua_State*);

        auto get() { return animation_.get(); }

    private:
        static int current_frame(lua_State*);
        static int is_stopped(lua_State*);
        static int set_delay(lua_State*);
        static int set_frame_rate(lua_State*);
        static int set_frames(lua_State*);
        static int set_listener(lua_State*);
        static int set_sprite(lua_State*);
        static int jump_to(lua_State*);
        static int rewind(lua_State*);
        static int start(lua_State*);
        static int stop(lua_State*);

        std::unique_ptr<rainbow::Animation> animation_;
        WeakRef listener_;
    };
} NS_RAINBOW_LUA_END

#endif
