// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"

#include <gtest/gtest.h>

#include "Graphics/SpriteBatch.h"
#include "Tests/TestHelpers.h"

using rainbow::Animation;
using rainbow::AnimationEvent;
using rainbow::Rect;
using rainbow::SpriteBatch;
using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::Vec2f;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureData;

namespace
{
    constexpr size_t kNumFrames = 4;
    constexpr uint64_t kFrameTime = 1000 / kNumFrames;

    auto mock_texture() { return TextureData{{}, 64, 64}; }

    class AnimationTest : public ::testing::Test
    {
    public:
        AnimationTest()
            : batch_(rainbow::ISolemnlySwearThatIAmOnlyTesting{}),
              sprite_(batch_.create_sprite(2, 2)),
              vertex_array_(batch_.vertices()),
              animation_(sprite_, nullptr, kNumFrames, 0)
        {
        }

    protected:
        std::array<Rect, kNumFrames + 1> frames_{
            Rect{0, 0, 16, 16},
            Rect{16, 16, 16, 16},
            Rect{32, 32, 16, 16},
            Rect{48, 48, 16, 16},
            Animation::end_frame(),
        };
        SpriteBatch batch_;
        SpriteRef sprite_;
        SpriteVertex* vertex_array_;
        std::array<Vec2f, 4> uv_;
        Animation animation_;

        void update()
        {
            auto span = ArraySpan<SpriteVertex>{vertex_array_, 4};
            sprite_->update(span, mock_texture());
        }

        void SetUp() override
        {
            sprite_->texture(frames_[0]);
            update();

            auto frames = std::make_unique<Rect[]>(frames_.size());
            std::copy(std::begin(frames_), std::end(frames_), frames.get());
            animation_.set_frames(std::move(frames));

            for (size_t i = 0; i < 4; ++i)
                uv_[i] = vertex_array_[i].texcoord;
        }
    };
}  // namespace

TEST_F(AnimationTest, IsStoppedInitially)
{
    ASSERT_EQ(animation_.current_frame(), 0U);
    ASSERT_TRUE(animation_.is_stopped());
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);
}

TEST_F(AnimationTest, AlwaysPlaysFromTheBeginning)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1U);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));

    animation_.stop();

    ASSERT_TRUE(animation_.is_stopped());
    ASSERT_EQ(animation_.current_frame(), 1U);

    animation_.start();
    update();

    ASSERT_EQ(animation_.current_frame(), 0U);
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);
}

TEST_F(AnimationTest, StoppedAnimationsDontProgressOnUpdate)
{
    animation_.update(kFrameTime * 2);
    update();

    ASSERT_EQ(animation_.current_frame(), 0U);
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
    ASSERT_EQ(animation_.current_frame(), 0U);
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
    ASSERT_EQ(animation_.current_frame(), 3U);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 3U);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 0U);
    animation_.update(kFrameTime * 3);
    ASSERT_EQ(animation_.current_frame(), 3U);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 3U);
    animation_.update(kFrameTime);
    ASSERT_EQ(animation_.current_frame(), 0U);
}

TEST_F(AnimationTest, ChangesFrameRate)
{
    animation_.start();

    ASSERT_EQ(animation_.current_frame(), 0U);

    animation_.update(kFrameTime);

    ASSERT_EQ(animation_.current_frame(), 1U);

    animation_.set_frame_rate(60);

    ASSERT_GE(animation_.frame_rate(), 60U);

    animation_.update(16);

    ASSERT_EQ(animation_.current_frame(), 2U);

    animation_.update(16);

    ASSERT_EQ(animation_.current_frame(), 3U);

    animation_.stop();
    animation_.set_frame_rate(1);
    animation_.start();
    animation_.update(1000);

    ASSERT_EQ(animation_.current_frame(), 1U);

    animation_.update(500);

    ASSERT_EQ(animation_.current_frame(), 1U);

    animation_.set_frame_rate(2);

    ASSERT_EQ(animation_.current_frame(), 1U);

    animation_.update(250);

    ASSERT_EQ(animation_.current_frame(), 2U);

    animation_.update(500);

    ASSERT_EQ(animation_.current_frame(), 3U);
}

TEST_F(AnimationTest, ResetsAnimationFrames)
{
    std::array<Rect, 3> kAnimationFrames{
        frames_[1],
        frames_[0],
        Animation::end_frame(),
    };

    animation_.start();

    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);

    animation_.set_frames(Animation::Frames(kAnimationFrames.data()));
    update();

    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));

    animation_.release();
}

TEST_F(AnimationTest, ResetsTargetSprite)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1U);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));

    sprite_->texture(frames_[0]);
    update();

    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);

    animation_.set_sprite(sprite_);
    update();

    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));
}

TEST_F(AnimationTest, JumpsToSpecificFrames)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1U);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));

    animation_.jump_to(0);
    update();

    ASSERT_EQ(animation_.current_frame(), 0U);
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array_[i].texcoord, uv_[i]);

    animation_.jump_to(1);
    update();

    ASSERT_EQ(animation_.current_frame(), 1U);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));
}

TEST_F(AnimationTest, Rewinds)
{
    animation_.start();
    animation_.update(kFrameTime);
    update();

    ASSERT_EQ(animation_.current_frame(), 1U);
    ASSERT_EQ(vertex_array_[0].texcoord, Vec2f(0.25F, 0.5F));
    ASSERT_EQ(vertex_array_[1].texcoord, Vec2f(0.5F, 0.5F));
    ASSERT_EQ(vertex_array_[2].texcoord, Vec2f(0.5F, 0.25F));
    ASSERT_EQ(vertex_array_[3].texcoord, Vec2f(0.25F, 0.25F));

    animation_.rewind();
    update();

    ASSERT_EQ(animation_.current_frame(), 0U);
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
                ASSERT_NE(a->current_frame(), 0U);
                if (++completed == 2)
                    a->stop();
                break;
            default:
                break;
        }
    });

    animation_.start();
    animation_.update(kFrameTime * 3);

    ASSERT_EQ(completed, 0U);
    ASSERT_EQ(animation_.current_frame(), 3U);

    animation_.update(kFrameTime);

    ASSERT_EQ(completed, 1U);
    ASSERT_EQ(animation_.current_frame(), 0U);

    animation_.update(kFrameTime * 4);

    ASSERT_EQ(completed, 2U);
    ASSERT_TRUE(animation_.is_stopped());
    ASSERT_EQ(animation_.current_frame(), 3U);

    animation_.start();

    ASSERT_EQ(animation_.current_frame(), 0U);
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
