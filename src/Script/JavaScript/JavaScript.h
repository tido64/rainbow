// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_DUKTAPESCRIPT_H_
#define SCRIPT_JAVASCRIPT_DUKTAPESCRIPT_H_

#include <duktape.h>

#include "Input/InputListener.h"
#include "Script/GameBase.h"

namespace rainbow
{
    namespace duk
    {
        class Context : private NonCopyable<Context>
        {
        public:
            Context();
            ~Context();

            explicit operator bool() const { return is_valid(); }
            operator duk_context*() { return context_; }

        private:
            duk_context* context_;

            auto is_valid() const -> bool { return context_ != nullptr; }
        };
    }  // namespace duk

    class JavaScript final : public GameBase, public InputListener
    {
    public:
        JavaScript(Director& director);

        auto context() -> duk_context* { return context_; }

    private:
        duk::Context context_;
        bool has_pointer_events_;

        bool update_controller_id(unsigned int i);

        // GameBase implementation details.

        void init_impl(const Vec2i& screen_size) override;
        void update_impl(uint64_t) override;
        void on_memory_warning_impl() override;

        // InputListener implementation details.

        bool on_controller_connected_impl(unsigned int i) override;
        bool on_controller_disconnected_impl(unsigned int i) override;
        bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_canceled_impl() override;
        bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
    };
}  // namespace rainbow

#endif
