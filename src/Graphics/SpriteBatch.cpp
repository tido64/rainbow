// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <algorithm>

#include "Graphics/Drawable.h"
#include "Graphics/Renderer.h"
#include "Graphics/ShaderDetails.h"
#include "Graphics/SpriteBatch.h"

namespace
{
	class Update
	{
	public:
		Update() : update(false) { }

		void operator()(Sprite &s)
		{
			this->update |= s.update();
		}

		explicit operator bool() const
		{
			return this->update;
		}

	private:
		bool update;
	};

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

	void set_buffer(Sprite &sprite, SpriteVertex *buffer)
	{
		sprite.set_vertex_array(buffer);
	}

	void set_buffer(Sprite &sprite, Vec2f *buffer)
	{
		sprite.set_normal_buffer(buffer);
	}

	template<typename T>
	void assign(Vector<Sprite> &sprites, Vector<T> &buffer)
	{
		// If the buffer was resized, we'll need to reassign all.
		if (push_back<T>(buffer))
		{
			for (size_t i = 0; i < sprites.size(); ++i)
				set_buffer(sprites[i], &buffer[i * 4]);
		}
		else  // Assign the buffer to the last sprite.
			set_buffer(sprites.back(), &buffer[buffer.size() - 4]);
	}
}

const char Drawable::class_name[] = "Drawable";

SpriteBatch::SpriteBatch(const size_t hint)
    : sprites_(hint), vertices_(hint * 4),
      array_([this]() {
      	this->texture_->bind();
      	this->vertices_.bind();
      	if (this->normal_.get())
      	{
      		this->normal_->bind(1);
      		this->normals_.bind(Shader::kAttributeNormal);
      	}
      }) { }

void SpriteBatch::set_normal(TextureAtlas *texture)
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

	assign(this->sprites_, this->vertices_.storage());
	R_ASSERT(this->sprites_.size() * 4 == this->vertices_.storage().size(),
	         "Sprite and vertex buffer are unsynchronized");

	if (this->normal_.get())
	{
		assign(this->sprites_, this->normals_.storage());
		R_ASSERT(this->normals_.storage().size() ==
		             this->vertices_.storage().size(),
		         "Normal and vertex buffer are unsynchronized");
	}

	return idx;
}

void SpriteBatch::update()
{
	// Update all sprites, then upload the vertex buffer if necessary.
	if (std::for_each(this->sprites_.begin(), this->sprites_.end(), Update()))
	{
		this->vertices_.commit();
		this->normals_.commit();
	}
}
