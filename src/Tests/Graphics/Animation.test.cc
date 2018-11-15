// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"

#include <gtest/gtest.h>

#include "Graphics/SpriteBatch.h"
#include "Tests/TestHelpers.h"

using rainbow::Animation;
using rainbow::AnimationEvent;
using rainbow::SharedPtr;
using rainbow::SpriteBatch;
using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::TextureAtlas;
using rainbow::Vec2f;

namespace
{
    constexpr size_t kNumFrames = 4;
    constexpr uint64_t kFrameTime = 1000 / kNumFrames;

    class AnimationTest : public ::testing::Test
    {
    public:
        AnimationTest()
            : atlas_(rainbow::make_shared<TextureAtlas>(mock)), batch_(mock),
              sprite_(batch_.create_sprite(2, 2)),
              vertex_array_(batch_.vertices()),
              animation_(sprite_, nullptr, kNumFrames, 0)
        {
        }

    protected:
        rainbow::ISolemnlySwearThatIAmOnlyTesting mock;
        SharedPtr<TextureAtlas> atlas_;
        SpriteBatch batch_;
        SpriteRef sprite_;
        SpriteVertex* vertex_array_;
        Vec2f uv_[4];
        Animation animation_;

        void update()
        {
            sprite_->update(ArraySpan<SpriteVertex>{vertex_array_, 4}, *atlas_);
        }

        void SetUp() override
        {
            Animation::Frames frames(new Animation::Frame[kNumFrames + 1]{
                atlas_->add_region(0, 0, 16, 16),
                atlas_->add_region(16, 16, 16, 16),
                atlas_->add_region(32, 32, 16, 16),
                atlas_->add_region(48, 48, 16, 16),
                Animation::Frame::end()});
            batch_.set_texture(atlas_);
            sprite_->set_texture(static_cast<uint32_t>(frames[0]));
            update();
            animation_.set_frames(std::move(frames));
            for (size_t i = 0; i < 4; ++i)
                uv_[i] = vertex_array_[i].texcoord;
        }
    };
}  // namespace

TEST_F(AnimationTest, IsStoppedInitially)
{
    ASSERT_EQ(animation_.current_frame(), 0u);
    ASSERT_TRUE(animation_.is_stopped());
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);
}

TEST_F(AnimationTest, AlwaysPlaysFromTheBeginning)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1u);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));

    animation_.stop();

    ASSERT_TRUE(animation_.is_stopped());
    ASSERT_EQ(animation_.current_frame(), 1u);

    animation_.start();
    update();

    ASSERT_EQ(animation_.current_frame(), 0u);
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);
}

TEST_F(AnimationTest, StoppedAnimationsDontProgressOnUpdate)
{
    animation_.update(kFrameTime * 2);
    update();

    ASSERT_EQ(animation_.current_frame(), 0u);
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);
}

TEST_F(AnimationTest, LoopsByDefault)
{
    animation_.start();
    ASSERT_FALSE(animation_.is_stopped());
    for (unsigned int i = 0; i < kNumFrames * 2; ++i)
    {
        ASSERT_EQ(animation_.current_frame(), i % kNumFrames);
        animation_.update(kFrameTime);
    }
    ASSERT_EQ(animation_.current_frame(), 0u);
}

TEST_F(AnimationTest, DisablesLooping)
{
    animation_.set_delay(-1);
    animation_.start();
    for (unsigned int i = 0; i < kNumFrames * 2; ++i)
    {
        ASSERT_EQ(animation_.current_frame(),
                  std::min<unsigned int>(i, kNumFrames - 1));
        animation_.update(kFrameTime);
    }
}

TEST_F(AnimationTest, SetsDelayBetweenLoops)
{
    animation_.set_delay(1);
    animation_.start();
    animation_.update(kFrameTime * 3);
    ASSERT_EQ(animation_.current_frame(), 3u);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 3u);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 0u);
    animation_.update(kFrameTime * 3);
    ASSERT_EQ(animation_.current_frame(), 3u);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 3u);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 0u);
}

TEST_F(AnimationTest, ChangesFrameRate)
{
    animation_.start();

    ASSERT_EQ(animation_.current_frame(), 0u);

    animation_.update(kFrameTime);

    ASSERT_EQ(animation_.current_frame(), 1u);

    animation_.set_frame_rate(60);

    ASSERT_GE(animation_.frame_rate(), 60u);

    animation_.update(16);

    ASSERT_EQ(animation_.current_frame(), 2u);

    animation_.update(16);

    ASSERT_EQ(animation_.current_frame(), 3u);

    animation_.stop();
    animation_.set_frame_rate(1);
    animation_.start();
    animation_.update(1000);

    ASSERT_EQ(animation_.current_frame(), 1u);

    animation_.update(500);

    ASSERT_EQ(animation_.current_frame(), 1u);

    animation_.set_frame_rate(2);

    ASSERT_EQ(animation_.current_frame(), 1u);

    animation_.update(250);

    ASSERT_EQ(animation_.current_frame(), 2u);

    animation_.update(500);

    ASSERT_EQ(animation_.current_frame(), 3u);
}

TEST_F(AnimationTest, ResetsAnimationFrames)
{
    Animation::Frame kAnimationFrames[3]{1, 0, Animation::Frame::end()};

    animation_.start();

    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);

    animation_.set_frames(Animation::Frames(kAnimationFrames));
    update();

    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));

    animation_.release();
}

TEST_F(AnimationTest, ResetsTargetSprite)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1u);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));

    sprite_->set_texture(0);
    update();

    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);

    animation_.set_sprite(sprite_);
    update();

    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));
}

TEST_F(AnimationTest, JumpsToSpecificFrames)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1u);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));

    animation_.jump_to(0);
    update();

    ASSERT_EQ(animation_.current_frame(), 0u);
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);

    animation_.jump_to(1);
    update();

    ASSERT_EQ(animation_.current_frame(), 1u);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));
}

TEST_F(AnimationTest, Rewinds)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1u);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25f, 0.5f));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5f, 0.5f));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5f, 0.25f));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25f, 0.25f));

    animation_.rewind();
    update();

    ASSERT_EQ(animation_.current_frame(), 0u);
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);
}

TEST_F(AnimationTest, EventCallbacksAreCalledOnlyOnce)
{
    int started = 0;
    int stopped = 0;
    int completed = 0;
    int frame = 0;
    animation_.set_callback([&](Animation* a, AnimationEvent e) {
        ASSERT_EQ(&animation_, a);
        switch (e)
        {
            case AnimationEvent::Start:
                ++started;
                break;
            case AnimationEvent::End:
                ++stopped;
                break;
            case AnimationEvent::Complete:
                ++completed;
                break;
            case AnimationEvent::Frame:
                ++frame;
                break;
            default:
                break;
        }
    });

    animation_.stop();

    ASSERT_EQ(started, 0);
    ASSERT_EQ(stopped, 0);

    animation_.start();

    ASSERT_EQ(started, 1);
    ASSERT_EQ(frame, 0);

    animation_.start();

    ASSERT_EQ(started, 1);
    ASSERT_EQ(frame, 0);

    animation_.update(kFrameTime * 3);

    ASSERT_EQ(started, 1);
    ASSERT_EQ(stopped, 0);
    ASSERT_EQ(completed, 0);
    ASSERT_EQ(frame, 3);

    animation_.update(kFrameTime);

    ASSERT_EQ(completed, 1);
    ASSERT_EQ(frame, 3);

    animation_.update(kFrameTime);

    ASSERT_EQ(completed, 1);
    ASSERT_EQ(frame, 4);

    animation_.stop();

    ASSERT_EQ(started, 1);
    ASSERT_EQ(stopped, 1);
    ASSERT_EQ(completed, 1);
    ASSERT_EQ(frame, 4);
}

TEST_F(AnimationTest, FiresCompleteEventAfterFrameChange)
{
    unsigned int completed = 0;
    animation_.set_callback([&completed](Animation* a, AnimationEvent e) {
        switch (e)
        {
            case AnimationEvent::Complete:
                ASSERT_NE(a->current_frame(), 0u);
                if (++completed == 2)
                    a->stop();
                break;
            default:
                break;
        }
    });

    animation_.start();
    animation_.update(kFrameTime * 3);

    ASSERT_EQ(completed, 0u);
    ASSERT_EQ(animation_.current_frame(), 3u);

    animation_.update(kFrameTime);

    ASSERT_EQ(completed, 1u);
    ASSERT_EQ(animation_.current_frame(), 0u);

    animation_.update(kFrameTime * 4);

    ASSERT_EQ(completed, 2u);
    ASSERT_TRUE(animation_.is_stopped());
    ASSERT_EQ(animation_.current_frame(), 3u);

    animation_.start();

    ASSERT_EQ(animation_.current_frame(), 0u);
}

TEST_F(AnimationTest, FiresFrameEventOnFrameChange)
{
    unsigned int frame = 0;
    animation_.set_callback([&frame](Animation* a, AnimationEvent e) {
        switch (e)
        {
            case AnimationEvent::Frame:
                ASSERT_EQ(a->current_frame(), frame);
                break;
            default:
                break;
        }
    });

    animation_.start();
    ++frame;
    animation_.update(kFrameTime);
    ++frame;
    animation_.update(kFrameTime);
    animation_.stop();
    animation_.start();
    frame = 1;
    animation_.update(kFrameTime);
}
