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
		    : atlas(make_shared<TextureAtlas>(mock)),
		      batch(mock), sprite(batch.create_sprite(2, 2)),
		      vertex_array(sprite->vertex_array()),
		      animation(sprite, nullptr, kNumFrames, 0) {}

	protected:
		rainbow::ISolemnlySwearThatIAmOnlyTesting mock;
		SharedPtr<TextureAtlas> atlas;
		SpriteBatch batch;
		Sprite::Ref sprite;
		const SpriteVertex* vertex_array;
		Vec2f u[4];
		Animation animation;

		void SetUp() override
		{
			Animation::Frames frames(new Animation::Frame[kNumFrames + 1]{
			    atlas->define(Vec2i::Zero, 16, 16),
			    atlas->define(Vec2i(16, 16), 16, 16),
			    atlas->define(Vec2i(32, 32), 16, 16),
			    atlas->define(Vec2i(48, 48), 16, 16),
			    Animation::kAnimationEnd});
			batch.set_texture(atlas);
			sprite->set_texture(frames[0]);
			animation.set_frames(std::move(frames));
			for (size_t i = 0; i < 4; ++i)
				u[i] = vertex_array[i].texcoord;
		}
	};
}

TEST_F(AnimationTest, IsStoppedInitially)
{
	ASSERT_EQ(0u, animation.current_frame());
	ASSERT_TRUE(animation.is_stopped());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);
}

TEST_F(AnimationTest, AlwaysPlaysFromTheBeginning)
{
	animation.start();
	animation.update(kFrameTime);

	ASSERT_EQ(1u, animation.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);

	animation.stop();

	ASSERT_TRUE(animation.is_stopped());
	ASSERT_EQ(1u, animation.current_frame());

	animation.start();

	ASSERT_EQ(0u, animation.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);
}

TEST_F(AnimationTest, StoppedAnimationsDontProgressOnUpdate)
{
	animation.update(kFrameTime * 2);
	ASSERT_EQ(0u, animation.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);
}

TEST_F(AnimationTest, LoopsByDefault)
{
	animation.start();
	ASSERT_FALSE(animation.is_stopped());
	for (unsigned int i = 0; i < kNumFrames * 2; ++i)
	{
		ASSERT_EQ(i % kNumFrames, animation.current_frame());
		animation.update(kFrameTime);
	}
	ASSERT_EQ(0u, animation.current_frame());
}

TEST_F(AnimationTest, DisablesLooping)
{
	animation.set_delay(-1);
	animation.start();
	for (unsigned int i = 0; i < kNumFrames * 2; ++i)
	{
		ASSERT_EQ(std::min<unsigned int>(i, kNumFrames - 1),
		          animation.current_frame());
		animation.update(kFrameTime);
	}
}

TEST_F(AnimationTest, SetsDelayBetweenLoops)
{
	animation.set_delay(1);
	animation.start();
	animation.update(kFrameTime * 3);
	ASSERT_EQ(3u, animation.current_frame());
	animation.update(kFrameTime);
	ASSERT_EQ(3u, animation.current_frame());
	animation.update(kFrameTime);
	ASSERT_EQ(0u, animation.current_frame());
	animation.update(kFrameTime * 3);
	ASSERT_EQ(3u, animation.current_frame());
	animation.update(kFrameTime);
	ASSERT_EQ(3u, animation.current_frame());
	animation.update(kFrameTime);
	ASSERT_EQ(0u, animation.current_frame());
}

TEST_F(AnimationTest, ChangesPlaybackSpeed)
{
	animation.start();
	ASSERT_EQ(0u, animation.current_frame());
	animation.update(kFrameTime);
	ASSERT_EQ(1u, animation.current_frame());
	animation.set_fps(60);
	animation.update(16);
	ASSERT_EQ(2u, animation.current_frame());
	animation.update(16);
	ASSERT_EQ(3u, animation.current_frame());
}

TEST_F(AnimationTest, ResetsAnimationFrames)
{
	animation.start();

	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);

	animation.set_frames(Animation::Frames(
	    new Animation::Frame[3]{1, 0, Animation::kAnimationEnd}));

	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);
}

TEST_F(AnimationTest, ResetsTargetSprite)
{
	animation.start();
	animation.update(kFrameTime);

	ASSERT_EQ(1u, animation.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);

	sprite->set_texture(0);

	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);

	animation.set_sprite(sprite);

	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);
}

TEST_F(AnimationTest, JumpsToSpecificFrames)
{
	animation.start();
	animation.update(kFrameTime);

	ASSERT_EQ(1u, animation.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);

	animation.jump_to(0);

	ASSERT_EQ(0u, animation.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);

	animation.jump_to(1);

	ASSERT_EQ(1u, animation.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);
}

TEST_F(AnimationTest, Rewinds)
{
	animation.start();
	animation.update(kFrameTime);

	ASSERT_EQ(1u, animation.current_frame());
	ASSERT_EQ(Vec2f(0.25f, 0.5f), vertex_array[0].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.5f), vertex_array[1].texcoord);
	ASSERT_EQ(Vec2f(0.5f, 0.25f), vertex_array[2].texcoord);
	ASSERT_EQ(Vec2f(0.25f, 0.25f), vertex_array[3].texcoord);

	animation.rewind();

	ASSERT_EQ(0u, animation.current_frame());
	for (size_t i = 0; i < 4; ++i)
		ASSERT_EQ(u[i], vertex_array[i].texcoord);
}

TEST_F(AnimationTest, EventCallbacksAreCalledOnlyOnce)
{
	int started = 0;
	int stopped = 0;
	int completed = 0;
	int frame = 0;
	animation.set_callback([&](Animation* a, Animation::Event e) {
		ASSERT_EQ(a, &animation);
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

	animation.stop();

	ASSERT_EQ(0, started);
	ASSERT_EQ(0, stopped);

	animation.start();

	ASSERT_EQ(1, started);
	ASSERT_EQ(0, frame);

	animation.start();

	ASSERT_EQ(1, started);
	ASSERT_EQ(0, frame);

	animation.update(kFrameTime * 3);

	ASSERT_EQ(1, started);
	ASSERT_EQ(0, stopped);
	ASSERT_EQ(0, completed);
	ASSERT_EQ(3, frame);

	animation.update(kFrameTime);

	ASSERT_EQ(1, completed);
	ASSERT_EQ(3, frame);

	animation.update(kFrameTime);

	ASSERT_EQ(1, completed);
	ASSERT_EQ(4, frame);

	animation.stop();

	ASSERT_EQ(1, started);
	ASSERT_EQ(1, stopped);
	ASSERT_EQ(1, completed);
	ASSERT_EQ(4, frame);
}
