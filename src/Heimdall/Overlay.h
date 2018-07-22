// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include <deque>

#include "Graphics/Drawable.h"
#include "Input/InputListener.h"

namespace rainbow
{
    class Director;
}

namespace heimdall
{
    class Overlay final : public rainbow::IDrawable,
                          public rainbow::InputListener
    {
        static constexpr size_t kDataSampleSize = 100;

    public:
        Overlay(rainbow::Director& director)
            : enabled_(false), director_(director),
              frame_times_(kDataSampleSize), vmem_usage_(kDataSampleSize)
        {
        }

        ~Overlay();

        void initialize();

        auto is_enabled() const { return enabled_; }

        void disable() { enabled_ = false; }
        void enable() { enabled_ = true; }

    private:
        bool enabled_;
        rainbow::Director& director_;
        std::deque<uint64_t> frame_times_;
        std::deque<float> vmem_usage_;

        void draw_menu_bar();
        void draw_performance();
        void draw_render_queue();

        // IDrawable implementation details

        void draw_impl() override;
        void update_impl(uint64_t) override;

        // InputListener implementation details

        bool on_key_down_impl(const rainbow::KeyStroke&) override;
        bool on_key_up_impl(const rainbow::KeyStroke&) override;
        bool on_mouse_wheel_impl(const ArrayView<rainbow::Pointer>&) override;
        bool on_pointer_began_impl(const ArrayView<rainbow::Pointer>&) override;
        bool on_pointer_canceled_impl() override;
        bool on_pointer_ended_impl(const ArrayView<rainbow::Pointer>&) override;
        bool on_pointer_moved_impl(const ArrayView<rainbow::Pointer>&) override;
    };
}  // namespace heimdall

#endif
