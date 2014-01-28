// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Drawable.h"
#include "Graphics/ShaderDetails.h"
#include "Graphics/SpriteBatch.h"

namespace Renderer { extern const size_t kNumSprites; }

namespace
{
	template<typename T>
	bool push_back(Vector<T> &vector)
	{
		const size_t current_capacity = vector.capacity();
		vector.push_back(T());
		vector.push_back(T());
		vector.push_back(T());
		vector.push_back(T());
		return vector.capacity() != current_capacity;
	}

	template<void (*set)(Sprite&, void*), typename T>
	void assign(Vector<Sprite> &sprites, Vector<T> &buffer)
	{
		// If the buffer was resized, we'll need to reassign.
		if (push_back<T>(buffer))
		{
			for (size_t i = 0; i < sprites.size(); ++i)
				set(sprites[i], &buffer[i * 4]);
		}
		else  // Assign the buffer to the last sprite.
			set(sprites.back(), &buffer[buffer.size() - 4]);
	}

	void set_normal_buffer(Sprite &sprite, void *buf)
	{
		sprite.set_normal_buffer(static_cast<Vec2f*>(buf));
	}

	void set_vertex_array(Sprite &sprite, void *buf)
	{
		sprite.set_vertex_array(static_cast<SpriteVertex*>(buf));
	}
}

const char Drawable::class_name[] = "Drawable";

SpriteBatch::SpriteBatch(const size_t hint) :
	sprites_(hint), vertices_(hint * 4),
	array_([this]() {
		this->texture_->bind();
		this->vertices_.bind();
		if (this->normal_.get())
		{
			this->normal_->bind(1);
			this->normals_.bind(Shader::kAttributeNormal);
		}
	}) { }

TextureAtlas* SpriteBatch::set_normal(TextureAtlas *texture)
{
	const auto &vertex_buffer = this->vertices_.storage();
	auto &normal_buffer = this->normals_.storage();
	if (normal_buffer.size() < vertex_buffer.size())
	{
		normal_buffer.reserve(vertex_buffer.capacity());
		while (normal_buffer.size() < vertex_buffer.size())
			normal_buffer.push_back(Vec2f());
		for (size_t i = 0; i < this->sprites_.size(); ++i)
			this->sprites_[i].set_normal_buffer(&normal_buffer[i * 4]);

		R_ASSERT(normal_buffer.size() == vertex_buffer.size(),
		         "Normal and vertex buffer are unsynchronized");
	}
	this->normal_ = texture;
	return this->normal_.get();
}

TextureAtlas* SpriteBatch::set_texture(const DataMap &texture)
{
	this->texture_ = new TextureAtlas(texture);
	return this->texture_.get();
}

TextureAtlas* SpriteBatch::set_texture(TextureAtlas *texture)
{
	this->texture_ = texture;
	return this->texture_.get();
}

unsigned int SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	const unsigned int idx = this->create_sprite(w, h);
	this->sprites_[idx].set_texture(this->texture_->define(Vec2i(x, y), w, h));
	return idx;
}

unsigned int SpriteBatch::create_sprite(const unsigned int width,
                                        const unsigned int height)
{
	R_ASSERT(this->sprites_.size() <= Renderer::kNumSprites,
	         "Hard-coded limit reached");

	const unsigned int idx = this->sprites_.size();
	this->sprites_.push_back(Sprite(width, height, this));

	assign<set_vertex_array>(this->sprites_, this->vertices_.storage());
	R_ASSERT(this->sprites_.size() * 4 == this->vertices_.storage().size(),
	         "Sprite and vertex buffer are unsynchronized");

	if (this->normal_.get())
	{
		assign<set_normal_buffer>(this->sprites_, this->normals_.storage());
		R_ASSERT(this->normals_.storage().size() ==
		             this->vertices_.storage().size(),
		         "Normal and vertex buffer are unsynchronized");
	}

	return idx;
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	bool needs_update = false;
	for (auto &sprite : this->sprites_)
		needs_update |= sprite.update();

	// Update vertex buffer
	if (needs_update)
	{
		this->vertices_.commit();
		this->normals_.commit();
	}
}
