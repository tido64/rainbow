// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Drawable.h"
#include "Graphics/SpriteBatch.h"

namespace Renderer { extern const int kNumSprites; }

const char Drawable::class_name[] = "Drawable";

SpriteBatch::SpriteBatch(const size_t hint) :
	sprites_(hint), vertices_(hint * 4) { }

TextureAtlas* SpriteBatch::set_texture(const DataMap &texture)
{
	this->texture_ = new TextureAtlas(texture);
	return this->texture_.get();
}

TextureAtlas* SpriteBatch::set_texture(TextureAtlas *t)
{
	this->texture_ = t;
	return this->texture_.get();
}

unsigned int SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	const unsigned int idx = this->create_sprite(w, h);
	this->sprites_[idx].set_texture(this->texture_->define(Vec2i(x, y), w, h));
	return idx;
}

unsigned int SpriteBatch::create_sprite(const unsigned int width, const unsigned int height)
{
	R_ASSERT(this->sprites_.size() <= static_cast<size_t>(Renderer::kNumSprites),
	         "Hard-coded limit reached");

	const size_t current_capacity = this->vertices_.capacity();
	this->vertices_.push_back(SpriteVertex());
	this->vertices_.push_back(SpriteVertex());
	this->vertices_.push_back(SpriteVertex());
	this->vertices_.push_back(SpriteVertex());

	const unsigned int idx = this->sprites_.size();
	this->sprites_.push_back(Sprite(width, height, this));
	this->array_.count += 6;

	R_ASSERT(this->sprites_.size() * 4 == this->vertices_.size(),
	         "Sprite and vertex buffer are unsynchronized");

	// If the vertex buffer was resized, we'll need to reassign buffers.
	if (this->vertices_.capacity() != current_capacity)
	{
		for (size_t i = 0; i < this->sprites_.size(); ++i)
			this->sprites_[i].vertex_array_ = &this->vertices_[i * 4];
	}
	else  // Assign the batch's buffer to the sprite.
		this->sprites_[idx].vertex_array_ = &this->vertices_[this->vertices_.size() - 4];

	return idx;
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	bool needs_update = false;
	for (auto &sprite : this->sprites_)
		needs_update = sprite.update() || needs_update;

	// Update vertex buffer
	if (needs_update)
	{
		this->array_.update(this->vertices_.begin(),
		                    this->vertices_.size() * sizeof(SpriteVertex));
	}
}
