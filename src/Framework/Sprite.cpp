/*
 *  Sprite.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/7/10.
 *  Copyright 2010 Ninja Unicorn. All rights reserved.
 *
 */

#include "TextureAtlas.h"

static const unsigned char stale_pivot    = 0x01;
static const unsigned char stale_position = 0x02;
static const unsigned char stale_scale    = 0x04;
static const unsigned char stale_angle    = 0x08;

Sprite::Sprite(TextureAtlas *a, const unsigned int w, const unsigned int h) :
	width(w), height(h), stale(0xff), texture(a->get_name()), angle(0.0f),
	scale_x(1.0f), scale_y(1.0f), pivot_x(0.5f), pivot_y(0.5f),
	position_x(0.0f), position_y(0.0f),
	vertex_array(new float[sprite_vertex_array]), atlas(a)
{
	this->update();
}

Sprite::~Sprite()
{
	if (!this->batched)
		delete[] this->vertex_array;
}

void Sprite::draw()
{
	// Enable all colour channels on texture
	glColor4ub(0xff, 0xff, 0xff, 0xff);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glVertexPointer(2, GL_FLOAT, sprite_buffer_stride, this->vertex_array);
	glTexCoordPointer(2, GL_FLOAT, sprite_buffer_stride, &this->vertex_array[2]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, sprite_vertices);
}

void Sprite::rotate(const float r)
{
	if (equalf(r, this->angle)) return;

	this->angle = r;
	this->stale |= stale_angle | stale_scale;
}

void Sprite::scale(const float f)
{
	assert(f > 0.0f);
	if (equalf(f, this->scale_x) && equalf(f, this->scale_y)) return;

	this->scale_x = f;
	this->scale_y = f;

	this->stale |= stale_scale;
}

void Sprite::scale(const float fx, const float fy)
{
	assert(fx > 0.0f && fy > 0.0f);
	if (equalf(fx, this->scale_x) && equalf(fy, this->scale_y)) return;

	this->scale_x = fx;
	this->scale_y = fy;

	this->stale |= stale_scale;
}

void Sprite::set_pivot(const float x, const float y)
{
	assert(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f);
	if (equalf(x, this->pivot_x) && equalf(y, this->pivot_y)) return;

	this->pivot_x = x;
	this->pivot_y = y;

	this->stale |= stale_pivot | stale_scale;
}

void Sprite::set_position(const float x, const float y)
{
	if (equalf(x, this->position_x) && equalf(y, this->position_y)) return;

	this->position_dx = x - this->position_x;
	this->position_dy = y - this->position_y;
	this->position_x = x;
	this->position_y = y;

	this->stale |= stale_position;
}

void Sprite::set_texture(const unsigned int i)
{
	this->atlas->get_texture(i, this->vertex_array);
}

void Sprite::update()
{
	if (this->stale == 0) return;

	if (this->stale & stale_scale)
	{
		if (this->stale & stale_pivot)
		{
			this->origin[0] = this->width * pivot_x;
			this->origin[1] = this->height * (1 - pivot_y);

			this->origin[2] = this->origin[0] - this->width;
			this->origin[3] = this->origin[1];

			this->origin[4] = this->origin[0];
			this->origin[5] = this->origin[1] - this->height;

			this->origin[6] = this->origin[2];
			this->origin[7] = this->origin[5];
		}

		if (this->angle != 0.0f)
		{
			if (this->stale & stale_angle)
			{
				this->cos_r = cos(-this->angle);
				this->sin_r = sin(-this->angle);
			}

			const float sx_cos_r = this->scale_x * this->cos_r;
			const float sx_sin_r = this->scale_x * this->sin_r;
			const float sy_cos_r = this->scale_y * this->cos_r;
			const float sy_sin_r = this->scale_y * this->sin_r;

			this->vertex_array[0] = sx_cos_r * this->origin[0] - sx_sin_r * this->origin[1] + this->position_x;
			this->vertex_array[1] = sy_sin_r * this->origin[0] + sy_cos_r * this->origin[1] + this->position_y;

			this->vertex_array[4] = sx_cos_r * this->origin[2] - sx_sin_r * this->origin[3] + this->position_x;
			this->vertex_array[5] = sy_sin_r * this->origin[2] + sy_cos_r * this->origin[3] + this->position_y;

			this->vertex_array[8] = sx_cos_r * this->origin[4] - sx_sin_r * this->origin[5] + this->position_x;
			this->vertex_array[9] = sy_sin_r * this->origin[4] + sy_cos_r * this->origin[5] + this->position_y;

			this->vertex_array[12] = sx_cos_r * this->origin[6] - sx_sin_r * this->origin[7] + this->position_x;
			this->vertex_array[13] = sy_sin_r * this->origin[6] + sy_cos_r * this->origin[7] + this->position_y;
		}
		else
		{
			this->vertex_array[0] = this->scale_x * this->origin[0] + this->position_x;
			this->vertex_array[1] = this->scale_y * this->origin[1] + this->position_y;

			this->vertex_array[4] = this->scale_x * this->origin[2] + this->position_x;
			this->vertex_array[5] = this->scale_y * this->origin[3] + this->position_y;

			this->vertex_array[8] = this->scale_x * this->origin[4] + this->position_x;
			this->vertex_array[9] = this->scale_y * this->origin[5] + this->position_y;

			this->vertex_array[12] = this->scale_x * this->origin[6] + this->position_x;
			this->vertex_array[13] = this->scale_y * this->origin[7] + this->position_y;
		}

		this->stale = 0;
		return;
	}

	if (this->stale & stale_position)
	{
		this->vertex_array[0] += this->position_dx;
		this->vertex_array[1] += this->position_dy;

		this->vertex_array[4] += this->position_dx;
		this->vertex_array[5] += this->position_dy;

		this->vertex_array[8] += this->position_dx;
		this->vertex_array[9] += this->position_dy;

		this->vertex_array[12] += this->position_dx;
		this->vertex_array[13] += this->position_dy;
	}
	this->stale = 0;
}
