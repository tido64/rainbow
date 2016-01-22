// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_BUTTON_H_
#define HEIMDALL_BUTTON_H_

#include "Graphics/Label.h"

namespace heimdall
{
    class Button
    {
    public:
        Label& drawable() { return label_; }

        template <typename F>
        void set_action(F&& action)
        {
            action_ = std::move(action);
        }

        void set_color(Colorb color) { label_.set_color(color); }

        void set_font(SharedPtr<FontAtlas> font)
        {
            label_.set_font(std::move(font));
        }

        void set_position(const Vec2f& position)
        {
            label_.set_position(position);
        }

        void set_text(const char* text) { label_.set_text(text); }

        bool hit_test(const Vec2i& point) const
        {
            const Vec2f& position = label_.position();
            const int max_x = position.x + label_.width();
            const int max_y = position.y + label_.font().height();
            return point.x >= position.x && point.x <= max_x &&
                   point.y >= position.y && point.y <= max_y;
        }

        void press() const { action_(); }

    private:
        Label label_;
        std::function<void()> action_;
    };
}

#endif
