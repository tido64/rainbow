// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_RENDERQUEUE_H_
#define LUA_RENDERQUEUE_H_

#include "Common/NonCopyable.h"
#include "Graphics/RenderQueue.h"
#include "Lua/LuaBind.h"

namespace rainbow { namespace lua
{
    class RenderQueue : private Bind<RenderQueue>,
                        private NonCopyable<RenderQueue>
    {
    public:
        static constexpr bool is_constructible = false;
        static const char class_name[];
        static const luaL_Reg functions[];

        RenderQueue(lua_State*) {}

        RenderQueue(lua_State*, graphics::RenderQueue& queue)
            : render_queue_(std::addressof(queue))
        {
        }

    private:
        template <typename F>
        static auto invoke(lua_State*, F&& func) -> int;

        static auto add(lua_State*) -> int;
        static auto disable(lua_State*) -> int;
        static auto enable(lua_State*) -> int;
        static auto insert(lua_State*) -> int;
        static auto erase(lua_State*) -> int;
        static auto erase_if_tag_starts_with(lua_State*) -> int;
        static auto set_program(lua_State*) -> int;
        static auto set_tag(lua_State*) -> int;

        graphics::RenderQueue* render_queue_;

        auto emplace(graphics::RenderQueue::const_iterator pos,
                     lua_State*,
                     int index) -> int;
    };
}}

#endif
