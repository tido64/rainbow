/*
 *  SpriteBatch.h
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/11/10.
 *  Copyright 2010 Ninja Unicorn. All rights reserved.
 *
 */

#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include <cstring>

#include "Sprite.h"

static const unsigned int degen_vx_sz = sprite_vertex_sz * sizeof(float);

template<int N>
class SpriteBatch
{
public:
	SpriteBatch() : count(0)
	{
		assert(N > 0);
		glGenBuffers(1, &this->buffer);
	}

	void add(Sprite *s)
	{
		assert(this->count < N);

		unsigned int offset = 0;
		if (this->count == 0)
			this->texture = s->texture;
		else
		{
			// Make sure all the sprites use the same texture atlas
			assert(s->texture == this->texture);

			// Copy degenerate vertices
			offset = this->count * sprite_vertex_array;
			memcpy(&this->vertex_buffer[offset], &this->vertex_buffer[offset - sprite_vertex_sz], degen_vx_sz);
			offset += sprite_vertex_sz;

			// Copy the sprite
			memcpy(&this->vertex_buffer[offset], s->vertex_array, degen_vx_sz);
			offset += sprite_vertex_sz;
		}

		// Delete the current vertex array and assign the batch's buffer
		memcpy(&this->vertex_buffer[offset], s->vertex_array, sprite_buffer_sz);
		delete[] s->vertex_array;
		s->vertex_array = &this->vertex_buffer[offset];
		s->batched = true;
		this->sprites[this->count++] = s;
	}

	void draw()
	{
		static const unsigned int stride = 4 * sizeof(float);
		static const void *tex_offset = reinterpret_cast<float *>(0) + 2;

		// Enable all colour channels
		glColor4ub(0xff, 0xff, 0xff, 0xff);

		glBindTexture(GL_TEXTURE_2D, this->texture);

		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glVertexPointer(2, GL_FLOAT, stride, 0);
		glTexCoordPointer(2, GL_FLOAT, stride, tex_offset);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, batch_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void update()
	{
		// Update all sprite vertices
		for (int i = 0; i < this->count; ++i)
			this->sprites[i]->update();

		// Update degenerate vertices
		for (int i = 1; i < this->count; ++i)
		{
			unsigned int offset = i * sprite_vertex_array;
			memcpy(&this->vertex_buffer[offset], &this->vertex_buffer[offset - sprite_vertex_sz], degen_vx_sz);
			offset += sprite_vertex_sz;
			memcpy(&this->vertex_buffer[offset], &this->vertex_buffer[offset + sprite_vertex_sz], degen_vx_sz);
		}

		// Update vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glBufferData(GL_ARRAY_BUFFER, batch_buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
	}

private:
	static const unsigned int batch_vertices = N * 4 + (N - 1) * 2;
	static const unsigned int batch_buffer_sz = batch_vertices * sprite_vertex_sz * sizeof(float);

	int count;
	GLuint buffer;
	GLuint texture;
	float vertex_buffer[batch_vertices * sprite_vertex_sz];
	Sprite *sprites[N];
};

#endif
