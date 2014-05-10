// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

#include <algorithm>

#include "Graphics/Drawable.h"
#include "Graphics/Renderer.h"
#include "Graphics/ShaderDetails.h"

namespace
{
	template<typename T>
	class SetBuffer
	{
	public:
		explicit SetBuffer(T *buffer);
		void operator()(Sprite &sprite);

	private:
		T *buffer_;
	};

	class Update
	{
	public:
		Update();
		void operator()(Sprite &sprite);
		explicit operator bool() const;

	private:
		bool should_update_;
	};
}

const char Drawable::class_name[] = "Drawable";

SpriteBatch::SpriteBatch(const unsigned int hint) : count_(0), reserved_(0)
{
	resize(hint);
	array_.reconfigure(std::bind(&SpriteBatch::bind_arrays, this));
}

SpriteBatch::~SpriteBatch()
{
	sprites_.release(count_);
	vertices_.release(count_ * 4);
	normals_.release(count_ * 4);
}

void SpriteBatch::set_normal(TextureAtlas *texture)
{
	if (!normals_)
	{
		normals_.resize(0, reserved_ * 4);
		std::uninitialized_fill_n(normals_.get(), count_ * 4, Vec2f());
		set_buffer(normals_.get());
		array_.reconfigure(std::bind(&SpriteBatch::bind_arrays, this));
	}
	normal_ = texture;
}

void SpriteBatch::set_texture(TextureAtlas *texture)
{
	texture_ = texture;
}

void SpriteBatch::bind_textures() const
{
	if (normal_)
		normal_->bind(1);
	texture_->bind();
}

unsigned int
SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	const unsigned int idx = create_sprite(w, h);
	sprites_[idx].set_texture(texture_->define(Vec2i(x, y), w, h));
	return idx;
}

unsigned int SpriteBatch::create_sprite(const unsigned int width,
                                        const unsigned int height)
{
	R_ASSERT(count_ <= Renderer::kNumSprites, "Hard-coded limit reached");

	if (count_ == reserved_)
	{
		const unsigned int half = reserved_ / 2;
		resize(reserved_ + ((half == 0) ? 1 : half));
	}
	Sprite &sprite = sprites_[count_];
	new (&sprite) Sprite(width, height, this);
	const unsigned int offset = count_ * 4;
	std::uninitialized_fill_n(vertices_ + offset, 4, SpriteVertex());
	sprite.set_vertex_array(vertices_ + offset);
	if (normals_)
	{
		std::uninitialized_fill_n(normals_ + offset, 4, Vec2f());
		sprite.set_normal_buffer(normals_ + offset);
	}
	return count_++;
}

void SpriteBatch::move(const Vec2f &delta)
{
	if (delta.is_zero())
		return;

	std::for_each(sprites_.get(), sprites_ + count_, [&delta](Sprite &sprite) {
		sprite.move(delta);
	});
}


void SpriteBatch::update()
{
	// Update all sprites, then upload the vertex buffer if necessary.
	if (std::for_each(sprites_.get(), sprites_ + count_, Update()))
	{
		const unsigned int count = count_ * 4;
		vertex_buffer_.upload(vertices_.get(), count * sizeof(SpriteVertex));
		if (normals_)
			normal_buffer_.upload(normals_.get(), count * sizeof(Vec2f));
	}
}

void SpriteBatch::bind_arrays() const
{
	vertex_buffer_.bind();
	if (normals_)
		normal_buffer_.bind(Shader::kAttributeNormal);
}

void SpriteBatch::resize(const unsigned int size)
{
	sprites_.resize(count_, size);
	vertices_.resize(count_ * 4, size * 4);
	set_buffer(vertices_.get());
	if (normals_)
	{
		normals_.resize(count_ * 4, size * 4);
		set_buffer(normals_.get());
	}
	reserved_ = size;
}

template<typename T>
void SpriteBatch::set_buffer(T *buffer)
{
	std::for_each(sprites_.get(), sprites_ + count_, SetBuffer<T>(buffer));
}

template<typename T>
SetBuffer<T>::SetBuffer(T *buffer) : buffer_(buffer) { }

template<>
void SetBuffer<SpriteVertex>::operator()(Sprite &sprite)
{
	sprite.set_vertex_array(buffer_);
	buffer_ += 4;
}

template<>
void SetBuffer<Vec2f>::operator()(Sprite &sprite)
{
	sprite.set_normal_buffer(buffer_);
	buffer_ += 4;
}

Update::Update() : should_update_(false) { }

void Update::operator()(Sprite &sprite)
{
	should_update_ |= sprite.update();
}

Update::operator bool() const
{
	return should_update_;
}
