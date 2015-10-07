// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Graphics/Animation.h"
#include "Graphics/SpriteBatch.h"

namespace rainbow
{
	struct ISolemnlySwearThatIAmOnlyTesting {};
}

namespace
{
	constexpr size_t kNumFrames = 4;
	constexpr unsigned long kFrameTime = 1000 / kNumFrames;

	class AnimationTest : public ::testing::Test
	{
	public:
		AnimationTest()
		    : atlas_(make_shared<TextureAtlas>(mock)),
		      batch_(mock),
		      sprite_(batch_.create_sprite(2, 2)),
		      vertex_array_(sprite_->vertex_array()),
		      animation_(sprite_, nullptr, kNumFrames, 0) {}

	protected:
		rainbow::ISolemnlySwearThatIAmOnlyTesting mock;
		SharedPtr<TextureAtlas> atlas_;
		SpriteBatch batch_;
		Sprite::Ref sprite_;
		const SpriteVertex* vertex_array_;
		Vec2f uv_[4];
		Animation animation_;

		void SetUp() override
		{
			Animation::Frames frames(new Animation::Frame[kNumFrames + 1]{
			    atlas_->define(Vec2i::Zero, 16, 16),
			    atlas_->define(Vec2i(16, 16), 16, 16),
			    atlas_->define(Vec2i(32, 32), 16, 16),
			    atlas_->define(Vec2i(48, 48), 16, 16),
			    Animation::kAnimationEnd});
			batch_.set_texture(atlas_);
			sprite_->set_texture(frames[0]);
			animation_.set_frames(std::move(frames));
			for (size_t i = 0; i < 4; ++i)
				uv_[i] = vertex_array_[i].texcoord;
		}
	};
}

TEST_F(AnimationTest, IsStoppedInitially)
{
	ASSERT_EQ(0u, animation_.current_frame());
	ASSERT_TRUE(animation_.is_stopped());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);
}

TEST_F(AnimationTest, AlwaysPlaysFromTheBeginning)
{
	animation_.start();
	animation_.update(kFrameTime);

	ASSERT_EQ(1u, animation_.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);

	animation_.stop();

	ASSERT_TRUE(animation_.is_stopped());
	ASSERT_EQ(1u, animation_.current_frame());

	animation_.start();

	ASSERT_EQ(0u, animation_.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);
}

TEST_F(AnimationTest, StoppedAnimationsDontProgressOnUpdate)
{
	animation_.update(kFrameTime * 2);
	ASSERT_EQ(0u, animation_.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);
}

TEST_F(AnimationTest, LoopsByDefault)
{
	animation_.start();
	ASSERT_FALSE(animation_.is_stopped());
	for (unsigned int i = 0; i < kNumFrames * 2; ++i)
	{
		ASSERT_EQ(i % kNumFrames, animation_.current_frame());
		animation_.update(kFrameTime);
	}
	ASSERT_EQ(0u, animation_.current_frame());
}

TEST_F(AnimationTest, DisablesLooping)
{
	animation_.set_delay(-1);
	animation_.start();
	for (unsigned int i = 0; i < kNumFrames * 2; ++i)
	{
		ASSERT_EQ(std::min<unsigned int>(i, kNumFrames - 1),
		          animation_.current_frame());
		animation_.update(kFrameTime);
	}
}

TEST_F(AnimationTest, SetsDelayBetweenLoops)
{
	animation_.set_delay(1);
	animation_.start();
	animation_.update(kFrameTime * 3);
	ASSERT_EQ(3u, animation_.current_frame());
	animation_.update(kFrameTime);
	ASSERT_EQ(3u, animation_.current_frame());
	animation_.update(kFrameTime);
	ASSERT_EQ(0u, animation_.current_frame());
	animation_.update(kFrameTime * 3);
	ASSERT_EQ(3u, animation_.current_frame());
	animation_.update(kFrameTime);
	ASSERT_EQ(3u, animation_.current_frame());
	animation_.update(kFrameTime);
	ASSERT_EQ(0u, animation_.current_frame());
}

TEST_F(AnimationTest, ChangesFrameRate)
{
	animation_.start();

	ASSERT_EQ(0u, animation_.current_frame());

	animation_.update(kFrameTime);

	ASSERT_EQ(1u, animation_.current_frame());

	animation_.set_frame_rate(60);

	ASSERT_GE(animation_.frame_rate(), 60u);

	animation_.update(16);

	ASSERT_EQ(2u, animation_.current_frame());

	animation_.update(16);

	ASSERT_EQ(3u, animation_.current_frame());

	animation_.stop();
	animation_.set_frame_rate(1);
	animation_.start();
	animation_.update(1000);

	ASSERT_EQ(1u, animation_.current_frame());

	animation_.update(500);

	ASSERT_EQ(1u, animation_.current_frame());

	animation_.set_frame_rate(2);

	ASSERT_EQ(1u, animation_.current_frame());

	animation_.update(250);

	ASSERT_EQ(2u, animation_.current_frame());

	animation_.update(500);

	ASSERT_EQ(3u, animation_.current_frame());
}

TEST_F(AnimationTest, ResetsAnimationFrames)
{
	animation_.start();

	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);

	animation_.set_frames(Animation::Frames(
	    new Animation::Frame[3]{1, 0, Animation::kAnimationEnd}));

	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);
}

TEST_F(AnimationTest, ResetsTargetSprite)
{
	animation_.start();
	animation_.update(kFrameTime);

	ASSERT_EQ(1u, animation_.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);

	sprite_->set_texture(0);

	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);

	animation_.set_sprite(sprite_);

	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);
}

TEST_F(AnimationTest, JumpsToSpecificFrames)
{
	animation_.start();
	animation_.update(kFrameTime);

	ASSERT_EQ(1u, animation_.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);

	animation_.jump_to(0);

	ASSERT_EQ(0u, animation_.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);

	animation_.jump_to(1);

	ASSERT_EQ(1u, animation_.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);
}

TEST_F(AnimationTest, Rewinds)
{
	animation_.start();
	animation_.update(kFrameTime);

	ASSERT_EQ(1u, animation_.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array_[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array_[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array_[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array_[3].texcoord);

	animation_.rewind();

	ASSERT_EQ(0u, animation_.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(uv_[i], vertex_array_[i].texcoord);
}

TEST_F(AnimationTest, EventCallbacksAreCalledOnlyOnce)
{
	int started = 0;
	int stopped = 0;
	int completed = 0;
	int frame = 0;
	animation_.set_callback([&](Animation* a, Animation::Event e) {
		ASSERT_EQ(a, &animation_);
		switch (e)
		{
			case Animation::Event::Start:
				++started;
				break;
			case Animation::Event::End:
				++stopped;
				break;
			case Animation::Event::Complete:
				++completed;
				break;
			case Animation::Event::Frame:
				++frame;
				break;
			default:
				break;
		}
	});

	animation_.stop();

	ASSERT_EQ(0, started);
	ASSERT_EQ(0, stopped);

	animation_.start();

	ASSERT_EQ(1, started);
	ASSERT_EQ(0, frame);

	animation_.start();

	ASSERT_EQ(1, started);
	ASSERT_EQ(0, frame);

	animation_.update(kFrameTime * 3);

	ASSERT_EQ(1, started);
	ASSERT_EQ(0, stopped);
	ASSERT_EQ(0, completed);
	ASSERT_EQ(3, frame);

	animation_.update(kFrameTime);

	ASSERT_EQ(1, completed);
	ASSERT_EQ(3, frame);

	animation_.update(kFrameTime);

	ASSERT_EQ(1, completed);
	ASSERT_EQ(4, frame);

	animation_.stop();

	ASSERT_EQ(1, started);
	ASSERT_EQ(1, stopped);
	ASSERT_EQ(1, completed);
	ASSERT_EQ(4, frame);
}

TEST_F(AnimationTest, FiresCompleteEventAfterFrameChange)
{
	unsigned int completed = 0;
	animation_.set_callback([&completed](Animation* a, Animation::Event e) {
		switch (e)
		{
			case Animation::Event::Complete:
				ASSERT_NE(0u, a->current_frame());
				if (++completed == 2)
					a->stop();
				break;
			default:
				break;
		}
	});

	animation_.start();
	animation_.update(kFrameTime * 3);

	ASSERT_EQ(0u, completed);
	ASSERT_EQ(3u, animation_.current_frame());

	animation_.update(kFrameTime);

	ASSERT_EQ(1u, completed);
	ASSERT_EQ(0u, animation_.current_frame());

	animation_.update(kFrameTime * 4);

	ASSERT_EQ(2u, completed);
	ASSERT_TRUE(animation_.is_stopped());
	ASSERT_EQ(3u, animation_.current_frame());

	animation_.start();

	ASSERT_EQ(0u, animation_.current_frame());

}

TEST_F(AnimationTest, FiresFrameEventOnFrameChange)
{
	unsigned int frame = 0;
	animation_.set_callback([&frame](Animation* a, Animation::Event e) {
		switch (e)
		{
			case Animation::Event::Frame:
				ASSERT_EQ(frame, a->current_frame());
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
