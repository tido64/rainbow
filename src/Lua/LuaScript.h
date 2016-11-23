// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUASCRIPT_H_
#define LUA_LUASCRIPT_H_

#include "Input/InputListener.h"
#include "Lua/LuaMachine.h"
#include "Script/GameBase.h"

namespace rainbow
{
    class LuaScript final : public GameBase, public InputListener
    {
    public:
        LuaScript(Director& director) : GameBase(director), lua_({}) {}

        ~LuaScript() override;

        auto state() const -> lua_State* { return lua_; }

    private:
        LuaMachine lua_;

        // GameBase implementation details.

        void init_impl(const Vec2i& screen) override;
        void update_impl(uint64_t) override;
        void on_memory_warning_impl() override;

        // InputListener implementation details.

        bool on_key_down_impl(const KeyStroke&) override;
        bool on_key_up_impl(const KeyStroke&) override;

        bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_canceled_impl() override;
        bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
    };
}

#endif
