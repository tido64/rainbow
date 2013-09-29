// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/Drawable.h"
#include "Graphics/SpriteBatch.h"

namespace Renderer { extern const int kNumSprites; }

const char Drawable::class_name[] = "Drawable";

SpriteBatch::SpriteBatch(const size_t hint) :
	sprites(hint), vertices(hint * 4) { }

unsigned int SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	const unsigned int idx = this->create_sprite(w, h);
	this->sprites[idx].set_texture(this->texture->define(Vec2i(x, y), w, h));
	return idx;
}

unsigned int SpriteBatch::create_sprite(const unsigned int width, const unsigned int height)
{
	R_ASSERT(this->sprites.size() <= static_cast<size_t>(Renderer::kNumSprites),
	         "Hard-coded limit reached");

	const size_t current_capacity = this->vertices.capacity();
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());

	const unsigned int idx = this->sprites.size();
	this->sprites.push_back(Sprite(width, height, this));
	this->array.count += 6;

	R_ASSERT(this->sprites.size() * 4 == this->vertices.size(),
	         "Sprite and vertex buffer are unsynchronized");

	// If the vertex buffer was resized, we'll need to reassign buffers.
	if (this->vertices.capacity() != current_capacity)
	{
		for (size_t i = 0; i < this->sprites.size(); ++i)
			this->sprites[i].vertex_array_ = &this->vertices[i * 4];
	}
	else  // Assign the batch's buffer to the sprite.
		this->sprites[idx].vertex_array_ = &this->vertices[this->vertices.size() - 4];

	return idx;
}

TextureAtlas* SpriteBatch::set_texture(const DataMap &texture)
{
	this->texture = new TextureAtlas(texture);
	return this->texture.get();
}

TextureAtlas* SpriteBatch::set_texture(TextureAtlas *t)
{
	this->texture = t;
	return this->texture.get();
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	for (auto &sprite : this->sprites)
		sprite.update();

	// Update vertex buffer
	this->array.update(this->vertices.begin(), this->vertices.size() * sizeof(SpriteVertex));
}
