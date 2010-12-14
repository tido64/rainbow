/*
 *  Sprite.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/7/10.
 *  Copyright 2010 Ninja Unicorn. All rights reserved.
 *
 */

#include "TextureAtlas.h"

Sprite::Sprite(TextureAtlas *a, const unsigned int w, const unsigned int h) :
	width(w), height(h), texture(a->get_name()), rotation(0.0f),
	scale_x(1.0f), scale_y(1.0f), pivot_x(0.5f), pivot_y(0.5f),
	position_x(0), position_y(0), atlas(a)
{
	this->pivot_abs[0] = 0.0f;
	this->pivot_abs[1] = 0.0f;
	this->pivot_delta[0] = this->width * this->pivot_x;
	this->pivot_delta[1] = this->height * (1 - this->pivot_y);

	//glGenBuffers(1, &this->buffer);
	this->update();
}

void Sprite::draw()
{
	static const unsigned int stride = Sprite::vertices * sizeof(float);
	//static const void *tex_offset = reinterpret_cast<float *>(0) + 2;

	// Enables all colour channels
	glColor4ub(0xff, 0xff, 0xff, 0xff);

	// Loads texture into GPU memory.
	// Note: Need to check whether this is necessary every time.
	glBindTexture(GL_TEXTURE_2D, this->texture);

	//glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	//glVertexPointer(2, GL_FLOAT, stride, 0);
	//glTexCoordPointer(2, GL_FLOAT, stride, tex_offset);

	glVertexPointer(2, GL_FLOAT, stride, this->vertex_array);
	glTexCoordPointer(2, GL_FLOAT, stride, &this->vertex_array[2]);

	if (this->rotation != 0.0f)
	{
		glPushMatrix();
		glTranslatef(this->pivot_abs[0], this->pivot_abs[1], 0.0f);
		glRotatef(-this->rotation, 0.0f, 0.0f, 1.0f);
		glTranslatef(-this->pivot_abs[0], -this->pivot_abs[1], 0.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, Sprite::vertices);
		glPopMatrix();
	}
	else
		glDrawArrays(GL_TRIANGLE_STRIP, 0, Sprite::vertices);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sprite::set_position(const float x, const float y)
{
	if (x == this->position_x && y == this->position_y) return;

	this->pivot_abs[0] += x - this->position_x;
	this->pivot_abs[1] += y - this->position_y;
	this->position_x = x;
	this->position_y = y;

	this->update();
}

void Sprite::set_texture(const unsigned int id, GLenum usage)
{
	this->atlas->get_texture(id, this->vertex_array);

	//glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	//glBufferData(GL_ARRAY_BUFFER, Sprite::buffer_sz, this->vertices, usage);
}

void Sprite::update()
{
	const float width = this->width * this->scale_x;
	const float height = this->height * this->scale_y;

	this->vertex_array[ 0] = this->position_x + width - this->pivot_delta[0] * this->scale_x;
	this->vertex_array[ 1] = this->position_y + this->pivot_delta[1] * this->scale_y;

	this->vertex_array[ 4] = this->vertex_array[0] - width;
	this->vertex_array[ 5] = this->vertex_array[1];

	this->vertex_array[ 8] = this->vertex_array[0];
	this->vertex_array[ 9] = this->vertex_array[1] - height;

	this->vertex_array[12] = this->vertex_array[4];
	this->vertex_array[13] = this->vertex_array[9];

	//glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	//glBufferData(GL_ARRAY_BUFFER, Sprite::buffer_sz, this->vertices, GL_DYNAMIC_DRAW);
}
