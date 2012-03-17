// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Algorithm.h"
#include "Graphics/SpriteBatch.h"

using Rainbow::equalf;

const unsigned char stale_pivot    = 0x01;
const unsigned char stale_position = 0x02;
const unsigned char stale_scale    = 0x04;
const unsigned char stale_angle    = 0x08;

Sprite::Sprite(const unsigned int w, const unsigned int h, const SpriteBatch *p) :
	width(w), height(h), buffered(false), stale(0xff), angle(0.0f),
	cos_r(1.0f), sin_r(0.0f), vertex_array(nullptr), parent(p),
	pivot(0.5f, 0.5f), scale_f(1.0f, 1.0f) { }

Sprite::Sprite(const Sprite &s) :
	width(s.width), height(s.height), buffered(false), stale(s.stale),
	angle(s.angle), cos_r(s.cos_r), sin_r(s.sin_r),	vertex_array(nullptr),
	parent(s.parent), pivot(s.pivot), position(s.position),
	position_d(s.position_d), scale_f(s.scale_f)
{
	memcpy(this->origin, s.origin, 4 * sizeof(Vec2f));
}

void Sprite::move(const float x, const float y)
{
	if (equalf(x, 0.0f) && equalf(y, 0.0f))
		return;

	this->position_d.x += x;
	this->position_d.y += y;
	this->stale |= stale_position;
}

void Sprite::rotate(const float r)
{
	if (equalf(r, 0.0f))
		return;

	this->angle += r;
	this->stale |= stale_angle | stale_scale;
}

void Sprite::set_color(const unsigned int c)
{
	this->vertex_array[0].color = c;
	this->vertex_array[1].color = c;
	this->vertex_array[2].color = c;
	this->vertex_array[3].color = c;
}

void Sprite::set_pivot(const float x, const float y)
{
	R_ASSERT(x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f, "set_pivot: Invalid pivot point.");

	if (equalf(x, this->pivot.x) && equalf(y, this->pivot.y))
		return;

	this->pivot.x = x;
	this->pivot.y = y;
	this->stale |= stale_pivot | stale_scale;
}

void Sprite::set_position(const float x, const float y)
{
	if (equalf(x, this->position.x) && equalf(y, this->position.y))
		return;

	this->position_d.x = x;
	this->position_d.y = y;
	this->stale |= stale_position;
}

void Sprite::set_position(const Vec2f &p)
{
	if (equalf(p.x, this->position.x) && equalf(p.y, this->position.y))
		return;

	this->position_d = p;
	this->stale |= stale_position;
}

void Sprite::set_rotation(const float r)
{
	if (equalf(r, this->angle))
		return;

	this->angle = r;
	this->stale |= stale_angle | stale_scale;
}

void Sprite::set_scale(const float f)
{
	R_ASSERT(f > 0.0f, "set_scale: Can't scale with a negative factor.");

	if (equalf(f, this->scale_f.x) && equalf(f, this->scale_f.y))
		return;

	this->scale_f.x = f;
	this->scale_f.y = f;
	this->stale |= stale_scale;
}

void Sprite::set_scale(const Vec2f &f)
{
	R_ASSERT(f.x > 0.0f && f.y > 0.0f, "set_scale: Can't scale with a negative factor.");

	if (equalf(f.x, this->scale_f.x) && equalf(f.y, this->scale_f.y))
		return;

	this->scale_f = f;
	this->stale |= stale_scale;
}

void Sprite::set_texture(const unsigned int id)
{
	const Texture &tx = (*this->parent->texture)[id];
	for (unsigned int i = 0; i < 4; ++i)
		this->vertex_array[i].texcoord = tx.vx[i];
}

void Sprite::update()
{
	if (!this->stale)
		return;

	if (this->stale & stale_scale)
	{
		if (this->stale & stale_pivot)
		{
			this->origin[0].x = this->width * -this->pivot.x;
			this->origin[0].y = this->height * -(1 - this->pivot.y);
			this->origin[1].x = this->origin[0].x + this->width;
			this->origin[1].y = this->origin[0].y;
			this->origin[2].x = this->origin[1].x;
			this->origin[2].y = this->origin[1].y + this->height;
			this->origin[3].x = this->origin[0].x;
			this->origin[3].y = this->origin[2].y;
		}

		if (this->stale & stale_position)
		{
			this->position = this->position_d;
			this->position_d = this->position;
		}

		if (this->angle != 0.0f)
		{
			if (this->stale & stale_angle)
			{
				this->cos_r = cosf(-this->angle);
				this->sin_r = sinf(-this->angle);
			}

			const float sx_cos_r = this->scale_f.x * this->cos_r;
			const float sx_sin_r = this->scale_f.x * this->sin_r;
			const float sy_cos_r = this->scale_f.y * this->cos_r;
			const float sy_sin_r = this->scale_f.y * this->sin_r;

			this->vertex_array[0].position.x = sx_cos_r * this->origin[0].x - sx_sin_r * this->origin[0].y + this->position.x;
			this->vertex_array[0].position.y = sy_sin_r * this->origin[0].x + sy_cos_r * this->origin[0].y + this->position.y;

			this->vertex_array[1].position.x = sx_cos_r * this->origin[1].x - sx_sin_r * this->origin[1].y + this->position.x;
			this->vertex_array[1].position.y = sy_sin_r * this->origin[1].x + sy_cos_r * this->origin[1].y + this->position.y;

			this->vertex_array[2].position.x = sx_cos_r * this->origin[2].x - sx_sin_r * this->origin[2].y + this->position.x;
			this->vertex_array[2].position.y = sy_sin_r * this->origin[2].x + sy_cos_r * this->origin[2].y + this->position.y;

			this->vertex_array[3].position.x = sx_cos_r * this->origin[3].x - sx_sin_r * this->origin[3].y + this->position.x;
			this->vertex_array[3].position.y = sy_sin_r * this->origin[3].x + sy_cos_r * this->origin[3].y + this->position.y;
		}
		else
		{
			this->vertex_array[0].position.x = this->scale_f.x * this->origin[0].x + this->position.x;
			this->vertex_array[0].position.y = this->scale_f.y * this->origin[0].y + this->position.y;

			this->vertex_array[1].position.x = this->scale_f.x * this->origin[1].x + this->position.x;
			this->vertex_array[1].position.y = this->scale_f.y * this->origin[1].y + this->position.y;

			this->vertex_array[2].position.x = this->scale_f.x * this->origin[2].x + this->position.x;
			this->vertex_array[2].position.y = this->scale_f.y * this->origin[2].y + this->position.y;

			this->vertex_array[3].position.x = this->scale_f.x * this->origin[3].x + this->position.x;
			this->vertex_array[3].position.y = this->scale_f.y * this->origin[3].y + this->position.y;
		}
		this->stale = 0;
	}
	else
	{
		this->position_d -= this->position;
		this->vertex_array[0].position += this->position_d;
		this->vertex_array[1].position += this->position_d;
		this->vertex_array[2].position += this->position_d;
		this->vertex_array[3].position += this->position_d;
		this->position += this->position_d;
		this->position_d = this->position;
		this->stale = 0;
	}
}
