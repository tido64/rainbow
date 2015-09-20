// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Graphics/Animation.h"
#include "Graphics/SpriteBatch.h"

namespace rainbow
{
	struct ISolemnlySwearThatIAmOnlyTesting {};
}

TEST_CASE("Animation specification", "[animation]")
{
	const size_t kNumFrames = 4;
	const unsigned long kFrameTime = 1000 / kNumFrames;

	rainbow::ISolemnlySwearThatIAmOnlyTesting mock;

	auto atlas = make_shared<TextureAtlas>(mock);
	Animation::Frames frames(new Animation::Frame[kNumFrames + 1]{
	    atlas->define(Vec2i::Zero, 16, 16),
	    atlas->define(Vec2i(16, 16), 16, 16),
	    atlas->define(Vec2i(32, 32), 16, 16),
	    atlas->define(Vec2i(48, 48), 16, 16),
	    Animation::kAnimationEnd});

	SpriteBatch batch(mock);
	batch.set_texture(atlas);

	auto sprite = batch.create_sprite(2, 2);
	sprite->set_texture(frames[0]);
	auto vertex_array = sprite->vertex_array();
	const Vec2f u[]{vertex_array[0].texcoord,
	                vertex_array[1].texcoord,
	                vertex_array[2].texcoord,
	                vertex_array[3].texcoord};

	Animation animation(sprite, std::move(frames), kNumFrames, 0);

	SECTION("Animations are stopped initially")
	{
		REQUIRE(animation.current_frame() == 0);
		REQUIRE(animation.is_stopped());
		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);
	}

	SECTION("Animations always play from the beginning")
	{
		animation.start();
		animation.update(kFrameTime);

		REQUIRE(animation.current_frame() == 1);
		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));

		animation.stop();

		REQUIRE(animation.is_stopped());
		REQUIRE(animation.current_frame() == 1);

		animation.start();

		REQUIRE(animation.current_frame() == 0);
		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);
	}

	SECTION("Stopped animations don't progress on update")
	{
		animation.update(kFrameTime * 2);
		REQUIRE(animation.current_frame() == 0);
		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);
	}

	SECTION("Animations loop by default")
	{
		animation.start();
		REQUIRE_FALSE(animation.is_stopped());
		for (unsigned int i = 0; i < kNumFrames * 2; ++i)
		{
			REQUIRE(animation.current_frame() == i % kNumFrames);
			animation.update(kFrameTime);
		}
		REQUIRE(animation.current_frame() == 0);
	}

	SECTION("Can disable looping")
	{
		animation.set_delay(-1);
		animation.start();
		for (unsigned int i = 0; i < kNumFrames * 2; ++i)
		{
			REQUIRE(animation.current_frame() ==
			        std::min<unsigned int>(i, kNumFrames - 1));
			animation.update(kFrameTime);
		}
	}

	SECTION("Can set delay between loops")
	{
		animation.set_delay(1);
		animation.start();
		animation.update(kFrameTime * 3);
		REQUIRE(animation.current_frame() == 3);
		animation.update(kFrameTime);
		REQUIRE(animation.current_frame() == 3);
		animation.update(kFrameTime);
		REQUIRE(animation.current_frame() == 0);
		animation.update(kFrameTime * 3);
		REQUIRE(animation.current_frame() == 3);
		animation.update(kFrameTime);
		REQUIRE(animation.current_frame() == 3);
		animation.update(kFrameTime);
		REQUIRE(animation.current_frame() == 0);
	}

	SECTION("Can change playback speed")
	{
		animation.start();
		REQUIRE(animation.current_frame() == 0);
		animation.update(kFrameTime);
		REQUIRE(animation.current_frame() == 1);
		animation.set_fps(60);
		animation.update(16);
		REQUIRE(animation.current_frame() == 2);
		animation.update(16);
		REQUIRE(animation.current_frame() == 3);
	}

	SECTION("Can reset animation frames")
	{
		animation.start();

		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);

		animation.set_frames(Animation::Frames(
		    new Animation::Frame[3]{1, 0, Animation::kAnimationEnd}));

		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));
	}

	SECTION("Can reset target sprite")
	{
		animation.start();
		animation.update(kFrameTime);

		REQUIRE(animation.current_frame() == 1);
		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));

		sprite->set_texture(0);

		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);

		animation.set_sprite(sprite);

		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));
	}

	SECTION("Can jump to specific frames")
	{
		animation.start();
		animation.update(kFrameTime);

		REQUIRE(animation.current_frame() == 1);
		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));

		animation.jump_to(0);

		REQUIRE(animation.current_frame() == 0);
		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);

		animation.jump_to(1);

		REQUIRE(animation.current_frame() == 1);
		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));
	}

	SECTION("Can be rewound")
	{
		animation.start();
		animation.update(kFrameTime);

		REQUIRE(animation.current_frame() == 1);
		REQUIRE(vertex_array[0].texcoord == Vec2f(0.25f, 0.5f));
		REQUIRE(vertex_array[1].texcoord == Vec2f(0.5f, 0.5f));
		REQUIRE(vertex_array[2].texcoord == Vec2f(0.5f, 0.25f));
		REQUIRE(vertex_array[3].texcoord == Vec2f(0.25f, 0.25f));

		animation.rewind();

		REQUIRE(animation.current_frame() == 0);
		REQUIRE(vertex_array[0].texcoord == u[0]);
		REQUIRE(vertex_array[1].texcoord == u[1]);
		REQUIRE(vertex_array[2].texcoord == u[2]);
		REQUIRE(vertex_array[3].texcoord == u[3]);
	}

	SECTION("Event callbacks are called only once")
	{
		int started = 0;
		int stopped = 0;
		int completed = 0;
		int frame = 0;
		animation.set_callback([&](Animation *a, Animation::Event e) {
			REQUIRE(&animation == a);
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

		REQUIRE(started == 0);
		REQUIRE(stopped == 0);

		animation.start();

		REQUIRE(started == 1);
		REQUIRE(frame == 0);

		animation.start();

		REQUIRE(started == 1);
		REQUIRE(frame == 0);

		animation.update(kFrameTime * 3);

		REQUIRE(started == 1);
		REQUIRE(stopped == 0);
		REQUIRE(completed == 0);
		REQUIRE(frame == 3);

		animation.update(kFrameTime);

		REQUIRE(completed == 1);
		REQUIRE(frame == 3);

		animation.update(kFrameTime);

		REQUIRE(completed == 1);
		REQUIRE(frame == 4);

		animation.stop();

		REQUIRE(started == 1);
		REQUIRE(stopped == 1);
		REQUIRE(completed == 1);
		REQUIRE(frame == 4);
	}
}
