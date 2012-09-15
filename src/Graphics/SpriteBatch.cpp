// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"

SpriteBatch::SpriteBatch(const size_t hint) :
	array_object(0), buffer(0), batch_vertices(0), sprites(hint),
	vertices(hint << 2)
{
	Renderer::create_buffer(this->buffer, this->array_object);
}

SpriteBatch::SpriteBatch(const SpriteBatch &sb) :
	array_object(0), buffer(0), batch_vertices(0), sprites(sb.sprites.size()),
	vertices(sb.sprites.size() << 2)
{
	Renderer::create_buffer(this->buffer, this->array_object);
	for (size_t i = 0; i < sb.sprites.size(); ++i)
		this->push_back(new Sprite(*sb.sprites[i], this));
}

SpriteBatch::~SpriteBatch()
{
	for (size_t i = 0; i < this->sprites.size(); ++i)
		delete this->sprites[i];
	Renderer::delete_buffer(this->buffer, this->array_object);
}

Sprite* SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	Sprite *s = new Sprite(w, h, this);
	this->push_back(s);
	s->set_texture(this->texture->define(x, y, w, h));
	return s;
}

void SpriteBatch::draw()
{
	this->texture->bind();
#ifndef RAINBOW_ANDROID
	Renderer::draw_buffer(this->array_object, this->batch_vertices);
#else
	Renderer::draw_buffer(this->buffer, this->batch_vertices);
#endif
}

TextureAtlas* SpriteBatch::set_texture(const Data &texture)
{
	this->texture = new TextureAtlas(texture);
	return this->texture.raw_ptr();
}

TextureAtlas* SpriteBatch::set_texture(TextureAtlas *t)
{
	this->texture = t;
	return this->texture.raw_ptr();
}

void SpriteBatch::push_back(Sprite *s)
{
	R_ASSERT(this->sprites.size() <= 256, "Hard-coded limit reached");

	const size_t current_capacity = this->vertices.capacity();
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->vertices.push_back(SpriteVertex());
	this->sprites.push_back(s);
	this->batch_vertices += 6;

	R_ASSERT(this->sprites.size() << 2 == this->vertices.size(), "Sprite and vertex buffer are unsynchronized");

	// If the vertex buffer was resized, we'll need to reassign buffers.
	if (this->vertices.capacity() != current_capacity)
		for (size_t i = 0; i < this->sprites.size(); ++i)
			this->sprites[i]->vertex_array = &this->vertices[i << 2];
	else
		// Assign the batch's buffer to the sprite.
		s->vertex_array = &this->vertices[this->vertices.size() - 4];
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	for (size_t i = 0; i < this->sprites.size(); ++i)
		this->sprites[i]->update();

	// Update vertex buffer
	Renderer::update_buffer(this->buffer, this->vertices.size() * sizeof(SpriteVertex), this->vertices.begin());
}
