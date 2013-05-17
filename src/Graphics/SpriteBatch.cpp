// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"

const char Drawable::class_name[] = "Drawable";

SpriteBatch::SpriteBatch(const size_t hint) :
	sprites(hint), vertices(hint << 2) { }

SpriteBatch::SpriteBatch(const SpriteBatch &sb) :
	sprites(sb.sprites.size()), vertices(sb.sprites.size() << 2)
{
	for (size_t i = 0; i < this->vertices.capacity(); ++i)
		this->vertices.push_back(SpriteVertex());
	for (size_t i = 0; i < this->sprites.capacity(); ++i)
	{
		this->sprites.push_back(new Sprite(*sb.sprites[i], this));
		this->sprites[i]->vertex_array = &this->vertices[i << 2];
	}
	this->array.count = this->sprites.size() * 6;
}

SpriteBatch::~SpriteBatch()
{
	for (size_t i = 0; i < this->sprites.size(); ++i)
		delete this->sprites[i];
}

Sprite* SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	Sprite *s = this->create_sprite(w, h);
	s->set_texture(this->texture->define(Vec2i(x, y), w, h));
	return s;
}

Sprite* SpriteBatch::create_sprite(const unsigned int width, const unsigned int height)
{
	R_ASSERT(this->sprites.size() <= 256, "Hard-coded limit reached");

	const size_t current_capacity = this->vertices.capacity();
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());

	Sprite *s = new Sprite(width, height, this);
	this->sprites.push_back(s);
	this->array.count += 6;

	R_ASSERT(this->sprites.size() << 2 == this->vertices.size(),
	         "Sprite and vertex buffer are unsynchronized");

	// If the vertex buffer was resized, we'll need to reassign buffers.
	if (this->vertices.capacity() != current_capacity)
		for (size_t i = 0; i < this->sprites.size(); ++i)
			this->sprites[i]->vertex_array = &this->vertices[i << 2];
	else
		// Assign the batch's buffer to the sprite.
		s->vertex_array = &this->vertices[this->vertices.size() - 4];

	return s;
}

void SpriteBatch::draw()
{
	Renderer::draw(this->array, this->texture.get());
}

TextureAtlas* SpriteBatch::set_texture(const Data &texture)
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
	for (size_t i = 0; i < this->sprites.size(); ++i)
		this->sprites[i]->update();

	// Update vertex buffer
	this->array.update(this->vertices.begin(), this->vertices.size() * sizeof(SpriteVertex));
}
