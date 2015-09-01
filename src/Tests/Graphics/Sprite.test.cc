// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Graphics/SpriteBatch.h"

namespace rainbow
{
	struct ISolemnlySwearThatIAmOnlyTesting {};
}

TEST_CASE("Sprites are placed at (0,0) on creation", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	REQUIRE(sprite->position() == Vec2f::Zero);

	sprite->update();

	auto vertex_array = sprite->vertex_array();
	REQUIRE(vertex_array[0].position == Vec2f(-1, -1));
	REQUIRE(vertex_array[1].position == Vec2f(1, -1));
	REQUIRE(vertex_array[2].position == Vec2f(1, 1));
	REQUIRE(vertex_array[3].position == Vec2f(-1, 1));
}

TEST_CASE("Sprites can be colored", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	const Colorb color(0xec, 0xef, 0xf1);
	REQUIRE_FALSE(sprite->color() == color);
	sprite->set_color(color);
	REQUIRE(sprite->color() == color);

	auto vertex_array = sprite->vertex_array();
	for (size_t i = 0; i < 4; ++i)
		REQUIRE(vertex_array[i].color == color);
}

TEST_CASE("Sprites can be tagged", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	REQUIRE(sprite->id() == Sprite::kNoId);

	const int tag = 0xdeadbeef;
	sprite->set_id(tag);
	REQUIRE(sprite->id() == tag);
}

TEST_CASE(
    "Changing a sprite's pivot point does not change its perceived position",
    "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	REQUIRE(sprite->pivot() == Vec2f(0.5f, 0.5f));

	sprite->update();
	sprite->set_pivot(Vec2f::Zero);

	REQUIRE(sprite->pivot() == Vec2f::Zero);
	REQUIRE(sprite->position() == -Vec2f::One);

	auto vertex_array = sprite->vertex_array();
	REQUIRE(vertex_array[0].position == Vec2f(-1, -1));
	REQUIRE(vertex_array[1].position == Vec2f(1, -1));
	REQUIRE(vertex_array[2].position == Vec2f(1, 1));
	REQUIRE(vertex_array[3].position == Vec2f(-1, 1));

	sprite->set_position(Vec2f::Zero);
	sprite->update();

	REQUIRE(sprite->position() == Vec2f::Zero);
	REQUIRE(vertex_array[0].position == Vec2f(0, 0));
	REQUIRE(vertex_array[1].position == Vec2f(2, 0));
	REQUIRE(vertex_array[2].position == Vec2f(2, 2));
	REQUIRE(vertex_array[3].position == Vec2f(0, 2));
}

TEST_CASE("Sprites can be translated", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	sprite->set_position(Vec2f::One);
	sprite->update();

	REQUIRE(sprite->position() == Vec2f::One);

	auto vertex_array = sprite->vertex_array();
	REQUIRE(vertex_array[0].position == Vec2f(0, 0));
	REQUIRE(vertex_array[1].position == Vec2f(2, 0));
	REQUIRE(vertex_array[2].position == Vec2f(2, 2));
	REQUIRE(vertex_array[3].position == Vec2f(0, 2));

	sprite->move(-Vec2f::One);

	REQUIRE(sprite->position() == Vec2f::Zero);
}

TEST_CASE("Sprites can be rotated", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(4, 2);

	REQUIRE(sprite->angle() == 0.0f);

	auto vertex_array = sprite->vertex_array();
	for (int i = 0; i < 2; ++i)
	{
		const float scale = 1.0f + 0.5f * i;
		const float width = sprite->width() * scale;
		const float height = sprite->height() * scale;
		sprite->set_scale(scale);
		for (int deg = 0; deg <= 360; ++deg)
		{
			const float rad = rainbow::radians(deg);
			sprite->set_rotation(rad);

			REQUIRE(sprite->angle() == rad);

			sprite->update();
			const Vec2f p[]{vertex_array[0].position,
			                vertex_array[1].position,
			                vertex_array[2].position,
			                vertex_array[3].position};

			if (deg == 0 || deg == 360)
				REQUIRE(rainbow::is_equal<float>(p[2].x, scale + scale));
			else
				REQUIRE_FALSE(rainbow::is_equal<float>(p[2].x, scale + scale));
			REQUIRE(p[0] == -p[2]);
			REQUIRE(p[1] == -p[3]);
			REQUIRE(rainbow::is_equal<float>(p[0].distance(p[1]), width));
			REQUIRE(rainbow::is_equal<float>(p[1].distance(p[2]), height));
			REQUIRE(rainbow::is_equal<float>(p[2].distance(p[3]), width));
			REQUIRE(rainbow::is_equal<float>(p[3].distance(p[0]), height));
		}
	}
}

TEST_CASE("Sprites can be scaled", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	REQUIRE(sprite->scale() == Vec2f::One);

	sprite->set_scale(2);

	REQUIRE(sprite->scale() == Vec2f(2.0f, 2.0f));

	sprite->update();

	auto vertex_array = sprite->vertex_array();
	REQUIRE(vertex_array[0].position == Vec2f(-2, -2));
	REQUIRE(vertex_array[1].position == Vec2f(2, -2));
	REQUIRE(vertex_array[2].position == Vec2f(2, 2));
	REQUIRE(vertex_array[3].position == Vec2f(-2, 2));

	sprite->set_scale(1);

	REQUIRE(sprite->scale() == Vec2f::One);

	sprite->update();

	REQUIRE(vertex_array[0].position == Vec2f(-1, -1));
	REQUIRE(vertex_array[1].position == Vec2f(1, -1));
	REQUIRE(vertex_array[2].position == Vec2f(1, 1));
	REQUIRE(vertex_array[3].position == Vec2f(-1, 1));

	sprite->set_scale(Vec2f(1, 2));

	REQUIRE(sprite->scale() == Vec2f(1.0f, 2.0f));

	sprite->update();

	REQUIRE(vertex_array[0].position == Vec2f(-1, -2));
	REQUIRE(vertex_array[1].position == Vec2f(1, -2));
	REQUIRE(vertex_array[2].position == Vec2f(1, 2));
	REQUIRE(vertex_array[3].position == Vec2f(-1, 2));
}

TEST_CASE("Sprites can be flipped/mirrored", "[sprite]")
{
	rainbow::ISolemnlySwearThatIAmOnlyTesting mock;

	auto atlas = make_shared<TextureAtlas>(mock);
	auto texture = atlas->define(Vec2i::Zero, 1, 1);

	SpriteBatch batch(mock);
	batch.set_texture(atlas);

	auto sprite = batch.create_sprite(2, 2);
	sprite->set_texture(texture);
	sprite->update();

	auto vertex_array = sprite->vertex_array();
	const Vec2f p[]{vertex_array[0].position,
	                vertex_array[1].position,
	                vertex_array[2].position,
	                vertex_array[3].position};
	const Vec2f u[]{vertex_array[0].texcoord,
	                vertex_array[1].texcoord,
	                vertex_array[2].texcoord,
	                vertex_array[3].texcoord};

	SECTION("Sprites aren't flipped/mirrored initially")
	{
		// Do nothing.
	}

	SECTION("Sprites can be flipped vertically")
	{
		sprite->flip();

		REQUIRE(sprite->is_flipped());
		REQUIRE_FALSE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[3]);
		REQUIRE(vertex_array[1].texcoord == u[2]);
		REQUIRE(vertex_array[2].texcoord == u[1]);
		REQUIRE(vertex_array[3].texcoord == u[0]);

		sprite->set_texture(texture);

		REQUIRE(vertex_array[0].texcoord == u[3]);
		REQUIRE(vertex_array[1].texcoord == u[2]);
		REQUIRE(vertex_array[2].texcoord == u[1]);
		REQUIRE(vertex_array[3].texcoord == u[0]);

		sprite->flip();
	}

	SECTION("Sprites can be mirrored")
	{
		sprite->mirror();

		REQUIRE_FALSE(sprite->is_flipped());
		REQUIRE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[1]);
		REQUIRE(vertex_array[1].texcoord == u[0]);
		REQUIRE(vertex_array[2].texcoord == u[3]);
		REQUIRE(vertex_array[3].texcoord == u[2]);

		sprite->set_texture(texture);

		REQUIRE(vertex_array[0].texcoord == u[1]);
		REQUIRE(vertex_array[1].texcoord == u[0]);
		REQUIRE(vertex_array[2].texcoord == u[3]);
		REQUIRE(vertex_array[3].texcoord == u[2]);

		sprite->mirror();
	}

	SECTION("Sprites can be flipped and mirrored")
	{
		sprite->flip();
		sprite->mirror();

		REQUIRE(sprite->is_flipped());
		REQUIRE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[2]);
		REQUIRE(vertex_array[1].texcoord == u[3]);
		REQUIRE(vertex_array[2].texcoord == u[0]);
		REQUIRE(vertex_array[3].texcoord == u[1]);

		sprite->set_texture(texture);

		REQUIRE(vertex_array[0].texcoord == u[2]);
		REQUIRE(vertex_array[1].texcoord == u[3]);
		REQUIRE(vertex_array[2].texcoord == u[0]);
		REQUIRE(vertex_array[3].texcoord == u[1]);

		sprite->flip();

		REQUIRE_FALSE(sprite->is_flipped());
		REQUIRE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[1]);
		REQUIRE(vertex_array[1].texcoord == u[0]);
		REQUIRE(vertex_array[2].texcoord == u[3]);
		REQUIRE(vertex_array[3].texcoord == u[2]);

		sprite->flip();

		REQUIRE(sprite->is_flipped());
		REQUIRE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[2]);
		REQUIRE(vertex_array[1].texcoord == u[3]);
		REQUIRE(vertex_array[2].texcoord == u[0]);
		REQUIRE(vertex_array[3].texcoord == u[1]);

		sprite->mirror();

		REQUIRE(sprite->is_flipped());
		REQUIRE_FALSE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[3]);
		REQUIRE(vertex_array[1].texcoord == u[2]);
		REQUIRE(vertex_array[2].texcoord == u[1]);
		REQUIRE(vertex_array[3].texcoord == u[0]);

		sprite->flip();

		REQUIRE_FALSE(sprite->is_flipped());
		REQUIRE_FALSE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
		{
			REQUIRE(vertex_array[i].position == p[i]);
			REQUIRE(vertex_array[i].texcoord == u[i]);
		}

		sprite->mirror();
		sprite->flip();

		REQUIRE(sprite->is_flipped());
		REQUIRE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[2]);
		REQUIRE(vertex_array[1].texcoord == u[3]);
		REQUIRE(vertex_array[2].texcoord == u[0]);
		REQUIRE(vertex_array[3].texcoord == u[1]);

		sprite->mirror();

		REQUIRE(sprite->is_flipped());
		REQUIRE_FALSE(sprite->is_mirrored());
		for (int i = 0; i < 4; ++i)
			REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[0].texcoord == u[3]);
		REQUIRE(vertex_array[1].texcoord == u[2]);
		REQUIRE(vertex_array[2].texcoord == u[1]);
		REQUIRE(vertex_array[3].texcoord == u[0]);

		sprite->flip();
	}

	REQUIRE_FALSE(sprite->is_flipped());
	REQUIRE_FALSE(sprite->is_mirrored());
	for (int i = 0; i < 4; ++i)
	{
		REQUIRE(vertex_array[i].position == p[i]);
		REQUIRE(vertex_array[i].texcoord == u[i]);
	}
}

TEST_CASE("Sprites can be hidden", "[sprite]")
{
	SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	auto sprite = batch.create_sprite(2, 2);

	SECTION("Multiple calls to hide/show are effectively noops")
	{
		REQUIRE_FALSE(sprite->is_hidden());

		sprite->hide();
		REQUIRE(sprite->is_hidden());
		sprite->hide();
		REQUIRE(sprite->is_hidden());

		sprite->show();
		REQUIRE_FALSE(sprite->is_hidden());
		sprite->show();
		REQUIRE_FALSE(sprite->is_hidden());
	}

	SECTION("Hidden sprites are restored properly")
	{
		sprite->hide();
		sprite->update();

		auto vertex_array = sprite->vertex_array();
		REQUIRE(vertex_array[0].position == Vec2f::Zero);
		REQUIRE(vertex_array[1].position == Vec2f::Zero);
		REQUIRE(vertex_array[2].position == Vec2f::Zero);
		REQUIRE(vertex_array[3].position == Vec2f::Zero);

		sprite->show();
		sprite->update();

		REQUIRE(vertex_array[0].position == Vec2f(-1, -1));
		REQUIRE(vertex_array[1].position == Vec2f(1, -1));
		REQUIRE(vertex_array[2].position == Vec2f(1, 1));
		REQUIRE(vertex_array[3].position == Vec2f(-1, 1));
	}

	SECTION("Sprites can be moved while hidden")
	{
		sprite->hide();

		sprite->set_position(Vec2f::One);
		sprite->update();

		auto vertex_array = sprite->vertex_array();
		REQUIRE(vertex_array[0].position == Vec2f::Zero);
		REQUIRE(vertex_array[1].position == Vec2f::Zero);
		REQUIRE(vertex_array[2].position == Vec2f::Zero);
		REQUIRE(vertex_array[3].position == Vec2f::Zero);

		sprite->show();
		sprite->update();

		REQUIRE(vertex_array[0].position == Vec2f(0, 0));
		REQUIRE(vertex_array[1].position == Vec2f(2, 0));
		REQUIRE(vertex_array[2].position == Vec2f(2, 2));
		REQUIRE(vertex_array[3].position == Vec2f(0, 2));
	}
}
