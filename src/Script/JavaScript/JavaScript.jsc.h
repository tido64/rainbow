// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_JAVASCRIPT_JSC_H
#define SCRIPT_JAVASCRIPT_JAVASCRIPT_JSC_H

#include <JavaScriptCore/JavaScript.h>

#include "Input/InputListener.h"
#include "Script/GameBase.h"

namespace rainbow::jsc
{
    template <typename T, typename Deleter>
    using unique_ref = std::unique_ptr<std::remove_pointer_t<T>, Deleter>;

    struct JSContextGroupReleaser
    {
        void operator()(JSContextGroupRef group) const
        {
            JSContextGroupRelease(group);
        }
    };

    struct JSGlobalContextReleaser
    {
        void operator()(JSGlobalContextRef ctx) const
        {
            JSGlobalContextRelease(ctx);
        }
    };

    using UniqueContextGroupRef =
        unique_ref<JSContextGroupRef, JSContextGroupReleaser>;

    using UniqueGlobalContextRef =
        unique_ref<JSGlobalContextRef, JSGlobalContextReleaser>;
}  // namespace rainbow::jsc

namespace rainbow
{
    // https://karhm.com/JavaScriptCore_C_API/
    class JavaScript final : public GameBase, public InputListener
    {
    public:
        JavaScript(Director& director);

    private:
        jsc::UniqueContextGroupRef context_group_;
        jsc::UniqueGlobalContextRef global_context_;
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
