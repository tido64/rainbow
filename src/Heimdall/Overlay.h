// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include <deque>

#include "Graphics/Drawable.h"
#include "Graphics/SceneGraph.h"
#include "Input/InputListener.h"

namespace rainbow { struct Rect; }

namespace heimdall
{
    class Overlay final : public IDrawable, public InputListener
    {
    public:
        Overlay();
        ~Overlay() override;

        void initialize(rainbow::SceneNode& parent);

        auto is_enabled() const { return node_->is_enabled(); }
        auto node() const { return node_; }

        void disable() { node_->set_enabled(false); }

        void enable()
        {
            node_->set_enabled(true);
            pinned_ = true;
        }

        template <typename T>
        void add_child(T& component) const
        {
            node_->add_child(component);
        }

    private:
        rainbow::SceneNode* node_;
        std::deque<uint64_t> frame_times_;
        std::deque<float> vmem_usage_;
        bool pinned_;

        // IDrawable implementation details

        void draw_impl() override;
        void update_impl(uint64_t) override;

        // InputListener implementation details

        bool on_key_down_impl(const rainbow::KeyStroke&) override;
        bool on_key_up_impl(const rainbow::KeyStroke&) override;
        bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_canceled_impl() override;
        bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
    };
}

#endif
