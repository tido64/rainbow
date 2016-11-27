// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Graphics/Drawable.h"
#include "Graphics/RenderQueue.h"

using rainbow::graphics::RenderQueue;

namespace
{
    constexpr uint64_t kDeltaTime = 16;

    class TestDrawable : public rainbow::IDrawable
    {
    public:
        auto is_drawn() const { return drawn_; }
        auto is_updated() const { return updated_; }

    private:
        bool drawn_ = false;
        bool updated_ = false;

        void draw_impl() override
        {
            drawn_ = true;
        }

        void update_impl(uint64_t dt) override
        {
            updated_ = dt == kDeltaTime;
        }
    };
}

TEST(RenderQueueTest, UpdatesOnlyEnabledUnits)
{
    TestDrawable drawables[10];
    RenderQueue queue;
    for (auto&& drawable : drawables)
        queue.emplace_back(drawable);
    queue[1].disable();
    queue[7].disable();
    rainbow::graphics::update(queue, kDeltaTime);

    for (size_t i = 0; i < queue.size(); ++i)
    {
        if (queue[i].is_enabled())
            ASSERT_TRUE(drawables[i].is_updated());
        else
            ASSERT_FALSE(drawables[i].is_updated());

        ASSERT_FALSE(drawables[i].is_drawn());
    }
}
