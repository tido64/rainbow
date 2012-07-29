// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"

SpriteBatch::SpriteBatch(const size_t hint) :
	array_object(0), buffer(0), batch_vertices(0), reserved(hint << 2),
	vertex_buffer(nullptr), sprites(hint)
{
	this->vertex_buffer = new SpriteVertex[this->reserved];
	Renderer::create_buffer(this->buffer, this->array_object);
}

SpriteBatch::~SpriteBatch()
{
	for (size_t i = 0; i < this->sprites.size(); ++i)
		delete this->sprites[i];
	delete[] this->vertex_buffer;
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

SmartPtr<TextureAtlas>& SpriteBatch::set_texture(SmartPtr<TextureAtlas> &t)
{
	return this->texture = t;
}

void SpriteBatch::push_back(Sprite *s)
{
	R_ASSERT(this->sprites.size() <= 256, "Hard-coded limit reached");

	const size_t index = this->sprites.size() << 2;
	if (index >= this->reserved)
	{
		this->reserved <<= 1;
		SpriteVertex *batch = new SpriteVertex[this->reserved];
		memcpy(batch, this->vertex_buffer, index * sizeof(SpriteVertex));
		delete[] this->vertex_buffer;
		this->vertex_buffer = batch;

		// Assign new sprite buffers
		for (size_t i = 0; i < this->sprites.size(); ++i)
			this->sprites[i]->vertex_array = this->vertex_buffer + (i << 2);
	}

	// Assign the batch's buffer to the sprite
	s->vertex_array = &this->vertex_buffer[index];
	this->sprites.push_back(s);
	this->batch_vertices += 6;
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	for (size_t i = 0; i < this->sprites.size(); ++i)
		this->sprites[i]->update();

	// Update vertex buffer
	Renderer::update_buffer(this->buffer, (this->sprites.size() << 2) * sizeof(SpriteVertex), this->vertex_buffer);
}
