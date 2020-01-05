// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_DUKTAPESCRIPT_H_
#define SCRIPT_JAVASCRIPT_DUKTAPESCRIPT_H_

#include <duktape.h>

#include "Input/InputListener.h"
#include "Script/GameBase.h"

namespace rainbow::duk
{
    class Context : private NonCopyable<Context>
    {
    public:
        Context(void* udata);
        ~Context();

        operator duk_context*() const { return context_; }

    private:
        duk_context* context_;
    };
}  // namespace rainbow::duk

namespace rainbow
{
    class JavaScript final : public GameBase, public InputListener
    {
    public:
        JavaScript(Director& director);

        auto context() { return static_cast<duk_context*>(context_); }

    private:
        duk::Context context_;
        bool has_pointer_events_ = false;

        auto update_controller_id(uint32_t port) -> bool;

        // GameBase implementation details.

        void init_impl(const Vec2i& screen_size) override;
        void update_impl(uint64_t) override;
        void on_memory_warning_impl() override;

        // InputListener implementation details.

        auto on_controller_connected_impl(uint32_t port) -> bool override;
        auto on_controller_disconnected_impl(uint32_t port) -> bool override;
        auto on_pointer_began_impl(const ArrayView<Pointer>&) -> bool override;
        auto on_pointer_canceled_impl() -> bool override;
        auto on_pointer_ended_impl(const ArrayView<Pointer>&) -> bool override;
        auto on_pointer_moved_impl(const ArrayView<Pointer>&) -> bool override;
    };
}  // namespace rainbow

#endif
