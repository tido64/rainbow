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

template<int N>
class SpriteBatch
{
public:
	SpriteBatch() : count(0)
	{
		assert(N > 0);
		glGenBuffers(1, &this->buffer);
	}

	void add(const Sprite *s)
	{
		assert(this->count != N);

		if (this->count == 0)
			this->texture = s->texture;

		this->sprites[this->count++] = s;
	}

	void draw()
	{
		static const unsigned int stride = 4 * sizeof(float);
		static const unsigned int vertices = N * 4 + (N - 1) * 2;
		static const void *tex_offset = reinterpret_cast<float *>(0) + 2;

		// Enables all colour channels
		glColor4ub(0xff, 0xff, 0xff, 0xff);

		glBindTexture(GL_TEXTURE_2D, this->texture);

		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glVertexPointer(2, GL_FLOAT, stride, 0);
		glTexCoordPointer(2, GL_FLOAT, stride, tex_offset);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void update()
	{
		static const unsigned int degen_sz = Sprite::buffer_sz >> 2;
		static const unsigned int sprite_sz = Sprite::vertices * Sprite::vertex_sz;

		const Sprite *s = this->sprites[0];
		memcpy(&this->vertex_buffer[0], s->vertex_array, Sprite::buffer_sz);
		unsigned int c = sprite_sz;
		memcpy(&this->vertex_buffer[c], &s->vertex_array[12], degen_sz);
		for (int i = 1; i < N - 1; ++i)
		{
			c += Sprite::vertex_sz;
			s = this->sprites[i];
			memcpy(&this->vertex_buffer[c], s->vertex_array, degen_sz);
			c += Sprite::vertex_sz;
			memcpy(&this->vertex_buffer[c], s->vertex_array, Sprite::buffer_sz);
			c += sprite_sz;
			memcpy(&this->vertex_buffer[c], &s->vertex_array[12], degen_sz);
		}
		c += Sprite::vertex_sz;
		s = this->sprites[N - 1];
		memcpy(&this->vertex_buffer[c], s->vertex_array, degen_sz);
		c += Sprite::vertex_sz;
		memcpy(&this->vertex_buffer[c], s->vertex_array, Sprite::buffer_sz);

		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glBufferData(GL_ARRAY_BUFFER, buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
	}

private:
	static const unsigned int vertices = N * 4 + (N - 1) * 2;
	static const unsigned int buffer_sz = vertices * Sprite::vertex_sz * sizeof(float);

	int count;
	GLuint buffer;
	GLuint texture;
	float vertex_buffer[vertices * Sprite::vertex_sz];
	const Sprite *sprites[N];
};

#endif
