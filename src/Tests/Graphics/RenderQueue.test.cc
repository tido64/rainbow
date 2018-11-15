// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/RenderQueue.h"

#include <gtest/gtest.h>

#include "Graphics/Drawable.h"

using rainbow::graphics::RenderQueue;
using rainbow::graphics::RenderUnit;

namespace
{
    constexpr uint64_t kDeltaTime = 16;

    class TestDrawable : public rainbow::IDrawable
    {
    public:
        auto draw_count() const { return drawn_; }
        auto update_count() const { return updated_; }

    private:
        int drawn_ = 0;
        int updated_ = 0;

        void draw_impl() override { ++drawn_; }

        void update_impl(uint64_t dt) override
        {
            updated_ += dt != kDeltaTime ? 0 : 1;
        }
    };
}  // namespace

TEST(RenderQueueTest, UnitsHaveReasonableDefaults)
{
    const std::string tag2{"test"};
    TestDrawable drawables[2];
    RenderQueue queue{drawables[0]};
    queue.emplace_back(drawables[1], tag2);

    const auto& unit1 = queue.front();

    ASSERT_TRUE(unit1.is_enabled());
    ASSERT_TRUE(unit1.tag().empty());

    const auto& unit2 = queue.back();

    ASSERT_TRUE(unit2.is_enabled());
    ASSERT_EQ(unit2.tag(), tag2);
}

TEST(RenderQueueTest, SetsUnitTag)
{
    const std::string kRandomTag = "42";
    const std::string kSecureRandomTag = "4";

    TestDrawable drawables[2];
    RenderQueue queue{{drawables[0], kRandomTag}, drawables[1]};
    auto& unit1 = queue.front();
    auto& unit2 = queue.back();

    ASSERT_FALSE(unit1.tag().empty());
    ASSERT_EQ(unit1.tag(), kRandomTag);
    ASSERT_EQ(unit1, kRandomTag.c_str());
    ASSERT_EQ(kRandomTag.c_str(), unit1);
    ASSERT_TRUE(unit2.tag().empty());

    unit1.set_tag(kSecureRandomTag);

    ASSERT_FALSE(unit1.tag().empty());
    ASSERT_EQ(unit1.tag(), kSecureRandomTag);
    ASSERT_EQ(unit1, kSecureRandomTag.c_str());
    ASSERT_EQ(kSecureRandomTag.c_str(), unit1);
    ASSERT_TRUE(unit2.tag().empty());

    unit1.set_tag({});
    unit2.set_tag(kRandomTag);

    ASSERT_TRUE(unit1.tag().empty());
    ASSERT_FALSE(unit2.tag().empty());
    ASSERT_EQ(unit2.tag(), kRandomTag);
    ASSERT_EQ(unit2, kRandomTag.c_str());
    ASSERT_EQ(kRandomTag.c_str(), unit2);
    ASSERT_NE(unit2, unit1);

    unit1.set_tag({});
    unit2.set_tag({});

    ASSERT_NE(unit2, unit1);
}

TEST(RenderQueueTest, UpdatesOnlyEnabledUnits)
{
    TestDrawable drawables[10];
    RenderQueue queue;
    std::transform(std::begin(drawables),
                   std::end(drawables),
                   std::back_inserter(queue),
                   [](auto&& drawable) -> RenderUnit { return drawable; });
    queue[1].disable();
    queue[7].disable();
    rainbow::graphics::update(queue, kDeltaTime);

    for (size_t i = 0; i < queue.size(); ++i)
    {
        ASSERT_EQ(drawables[i].update_count(), queue[i].is_enabled() ? 1 : 0);
        ASSERT_EQ(drawables[i].draw_count(), 0);
    }

    queue[1].enable();
    queue[2].enable();
    queue[6].disable();
    rainbow::graphics::update(queue, kDeltaTime);

    ASSERT_EQ(drawables[0].update_count(), 2);
    ASSERT_EQ(drawables[1].update_count(), 1);
    ASSERT_EQ(drawables[2].update_count(), 2);
    ASSERT_EQ(drawables[3].update_count(), 2);
    ASSERT_EQ(drawables[4].update_count(), 2);
    ASSERT_EQ(drawables[5].update_count(), 2);
    ASSERT_EQ(drawables[6].update_count(), 1);
    ASSERT_EQ(drawables[7].update_count(), 0);
    ASSERT_EQ(drawables[8].update_count(), 2);
    ASSERT_EQ(drawables[9].update_count(), 2);

    ASSERT_TRUE(std::all_of(
        std::begin(drawables), std::end(drawables), [](auto&& drawable) {
            return drawable.draw_count() == 0;
        }));
}
