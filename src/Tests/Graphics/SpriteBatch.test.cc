// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Graphics/SpriteBatch.h"

namespace rainbow
{
	struct ISolemnlySwearThatIAmOnlyTesting {};
}

namespace
{
	template<size_t N>
	void set_sprite_ids(const Sprite::Ref (&refs)[N])
	{
		for (size_t i = 0; i < N; ++i)
			refs[i]->set_id(i + 1);

		for (size_t i = 0; i < N; ++i)
			REQUIRE((static_cast<size_t>(refs[i]->id()) == i + 1));
	}

	void update(SpriteBatch &batch, const size_t count)
	{
		auto sprites = batch.sprites();
		std::for_each(sprites, sprites + count, [](Sprite &sprite) {
			sprite.update();
		});
	}

	void verify_sprite_vertices(const Sprite &sprite,
	                            const SpriteVertex *vertices,
	                            const Vec2f offset)
	{
		const float half_width = sprite.width() * 0.5f;
		const float half_height = sprite.height() * 0.5f;
		REQUIRE(vertices[0].position ==
		        Vec2f(-half_width + offset.x, -half_height + offset.y));
		REQUIRE(vertices[1].position ==
		        Vec2f(half_width + offset.x, -half_height + offset.y));
		REQUIRE(vertices[2].position ==
		        Vec2f(half_width + offset.x, half_height + offset.y));
		REQUIRE(vertices[3].position ==
		        Vec2f(-half_width + offset.x, half_height + offset.y));
	}

	void verify_batch_integrity(const SpriteBatch &batch, const size_t count)
	{
		const auto sprites = batch.sprites();
		const SpriteVertex *vertices = batch.vertices();
		for (size_t i = 0; i < count; ++i)
		{
			auto vertex_array = vertices + i * 4;
			REQUIRE(sprites[i].vertex_array() == vertex_array);
			verify_sprite_vertices(sprites[i], vertex_array, Vec2f::Zero);
		}
	}
}

TEST_CASE("Sprites can be manipulated in batches", "[spritebatch]")
{
	SpriteBatch batch((rainbow::ISolemnlySwearThatIAmOnlyTesting()));
	Sprite::Ref refs[]{
	    batch.create_sprite(1, 1),
	    batch.create_sprite(2, 1),
	    batch.create_sprite(1, 2),
	    batch.create_sprite(2, 2)};

	const size_t count = rainbow::array_size(refs);
	const SpriteVertex *vertices = batch.vertices();

	SECTION("Sprites share a single buffer")
	{
		REQUIRE(batch.count() == count * 6);

		const auto sprites = batch.sprites();
		for (size_t i = 0; i < count; ++i)
		{
			REQUIRE(&(*refs[i]) == &sprites[i]);
			REQUIRE(refs[i]->vertex_array() == vertices + i * 4);
		}
	}

	SECTION("Sprites are placed at (0,0) on creation")
	{
		update(batch, count);
		for (size_t i = 0; i < count; ++i)
		{
			REQUIRE(refs[i]->position() == Vec2f::Zero);
			verify_sprite_vertices(*refs[i], vertices + i * 4, Vec2f::Zero);
		}
	}

	SECTION("Sprites can be brought to front")
	{
		set_sprite_ids(refs);
		update(batch, count);

		batch.bring_to_front(refs[1]);

		auto sprites = batch.sprites();
		REQUIRE(sprites[0].id() == 1);
		REQUIRE(sprites[1].id() == 3);
		REQUIRE(sprites[2].id() == 4);
		REQUIRE(sprites[3].id() == 2);

		verify_batch_integrity(batch, count);
	}

	SECTION("Sprites can be erased")
	{
		set_sprite_ids(refs);
		update(batch, count);

		batch.erase(batch.find_sprite_by_id(2));

		size_t length = count - 1;
		REQUIRE(batch.count() == length * 6);
		auto sprites = batch.sprites();
		REQUIRE(sprites[0].id() == 1);
		REQUIRE(sprites[1].id() == 3);
		REQUIRE(sprites[2].id() == 4);

		verify_batch_integrity(batch, length);

		batch.erase(batch.find_sprite_by_id(1));

		REQUIRE(batch.count() == --length * 6);
		REQUIRE(sprites[0].id() == 3);
		REQUIRE(sprites[1].id() == 4);

		verify_batch_integrity(batch, length);

		batch.erase(batch.find_sprite_by_id(4));

		REQUIRE(batch.count() == 6);
		REQUIRE(sprites[0].id() == 3);
		REQUIRE(sprites[0].vertex_array() == vertices);
		verify_sprite_vertices(sprites[0], vertices, Vec2f::Zero);

		batch.erase(batch.find_sprite_by_id(3));
		REQUIRE(batch.count() == 0);
	}

	SECTION("Sprites can be found by its id")
	{
		set_sprite_ids(refs);

		for (auto&& sprite : refs)
			REQUIRE(&(*sprite) == &(*batch.find_sprite_by_id(sprite->id())));
	}

	SECTION("Sprites can be moved in batches")
	{
		batch.move(Vec2f::One);
		update(batch, count);
		for (size_t i = 0; i < count; ++i)
			verify_sprite_vertices(*refs[i], vertices + i * 4, Vec2f::One);

		batch.move(-Vec2f::One);
		update(batch, count);
		verify_batch_integrity(batch, count);
	}

	SECTION("Sprites can swap positions")
	{
		set_sprite_ids(refs);
		update(batch, count);

		batch.swap(refs[0], refs[0]);

		auto sprites = batch.sprites();
		REQUIRE(sprites[0].id() == 1);
		REQUIRE(sprites[1].id() == 2);
		REQUIRE(sprites[2].id() == 3);
		REQUIRE(sprites[3].id() == 4);

		verify_batch_integrity(batch, count);

		batch.swap(refs[0], refs[3]);

		REQUIRE(sprites[0].id() == 4);
		REQUIRE(sprites[1].id() == 2);
		REQUIRE(sprites[2].id() == 3);
		REQUIRE(sprites[3].id() == 1);

		verify_batch_integrity(batch, count);

		batch.swap(refs[1], refs[2]);

		REQUIRE(sprites[0].id() == 4);
		REQUIRE(sprites[1].id() == 3);
		REQUIRE(sprites[2].id() == 2);
		REQUIRE(sprites[3].id() == 1);

		verify_batch_integrity(batch, count);
	}
}
