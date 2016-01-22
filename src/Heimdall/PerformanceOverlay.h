// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_PERFORMANCEOVERLAY_H_
#define HEIMDALL_PERFORMANCEOVERLAY_H_

#include <deque>
#include <tuple>

#include "Graphics/LyricalLabel.h"
#include "Heimdall/Button.h"
#include "ThirdParty/NanoVG/NanoVG.h"

namespace rainbow { class SceneNode; }

namespace heimdall
{
    /// <summary>Displays frames per second and VRAM used.</summary>
    class PerformanceOverlay final : public nvg::Canvas,
                                     private NonCopyable<PerformanceOverlay>
    {
    public:
        PerformanceOverlay();

        Button& button() { return button_; }
        rainbow::SceneNode* node() const { return node_; }

        void init_button(const Vec2f& position, SharedPtr<FontAtlas> font);
        void init_graph(SharedPtr<FontAtlas> font);

        void set_origin(const Vec2f& origin);

    private:
        rainbow::SceneNode* node_;
        Vec2f position_;
        std::deque<std::tuple<float, float>> frame_data_;
        LyricalLabel labels_;
        float vmem_top_;
        Button button_;

        // Implement Drawable.

        void update_impl(unsigned long dt) override;

        // Implement nvg::Canvas.

        void paint_impl() override;
    };
}

#endif
