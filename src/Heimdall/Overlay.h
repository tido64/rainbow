// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include <deque>

#include "Graphics/Drawable.h"
#include "Input/InputListener.h"
#include "Math/Vec2.h"

namespace rainbow
{
    class Director;
}  // namespace rainbow

namespace heimdall
{
    class Overlay final : public rainbow::IDrawable,
                          public rainbow::InputListener
    {
        static constexpr size_t kDataSampleSize = 100;

    public:
        Overlay(rainbow::Director& director)
            : director_(director), frame_times_(kDataSampleSize),
              vmem_usage_(kDataSampleSize)
        {
        }

        ~Overlay() override;

        void initialize(rainbow::graphics::Context&, rainbow::Vec2i resolution);

        [[nodiscard]] auto is_enabled() const { return enabled_; }

        void disable() { enabled_ = false; }
        void enable() { enabled_ = true; }

    private:
        bool enabled_ = false;
        rainbow::Director& director_;
        std::deque<uint64_t> frame_times_;
        std::deque<float> vmem_usage_;

        [[nodiscard]] auto surface_height() const;

        void draw_menu_bar();
        void draw_performance(float scale);
        void draw_render_queue(rainbow::GameBase&);
        void draw_startup_message();

        // IDrawable implementation details

        void draw_impl(rainbow::graphics::Context&) const override;
        void update_impl(rainbow::GameBase&, uint64_t) override;

        // InputListener implementation details

        auto on_key_down_impl(const rainbow::KeyStroke&) -> bool override;
        auto on_key_up_impl(const rainbow::KeyStroke&) -> bool override;

        auto on_mouse_wheel_impl(const ArrayView<rainbow::Pointer>&)
            -> bool override;

        auto on_pointer_began_impl(const ArrayView<rainbow::Pointer>&)
            -> bool override;

        auto on_pointer_canceled_impl() -> bool override;

        auto on_pointer_ended_impl(const ArrayView<rainbow::Pointer>&)
            -> bool override;

        auto on_pointer_moved_impl(const ArrayView<rainbow::Pointer>&)
            -> bool override;
    };
}  // namespace heimdall

#endif
