/*
 *  SpriteBatch.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/11/10.
 *  Copyright 2010 Ninja Unicorn. All rights reserved.
 *
 */

#include "SpriteBatch.h"

template<>
void SpriteBatch<1>::update()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, Sprite::buffer_sz, this->sprites[0]->vertex_array, GL_DYNAMIC_DRAW);
}

template<>
void SpriteBatch<2>::update()
{
	static const unsigned int degenerate = Sprite::buffer_sz >> 2;

	const Sprite *s = this->sprites[0];
	memcpy(&this->vertex_buffer[0], s->vertex_array, Sprite::buffer_sz);
	memcpy(&this->vertex_buffer[16], &s->vertex_array[12], degenerate);
	s = this->sprites[1];
	memcpy(&this->vertex_buffer[20], s->vertex_array, degenerate);
	memcpy(&this->vertex_buffer[24], s->vertex_array, Sprite::buffer_sz);

	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
}
