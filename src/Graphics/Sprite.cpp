// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/SpriteBatch.h"

using Rainbow::equal;
using Rainbow::swap;

namespace
{
	const unsigned int kStaleBuffer    = 1u << 0;
	const unsigned int kStalePosition  = 1u << 1;

	inline Vec2f transform_rst(const Vec2f &p, const Vec2f &s_sin_r, const Vec2f &s_cos_r, const Vec2f &center)
	{
		return Vec2f(s_cos_r.x * p.x - s_sin_r.x * p.y + center.x,
		             s_sin_r.y * p.x + s_cos_r.y * p.y + center.y);
	}

	inline Vec2f transform_st(const Vec2f &p, const Vec2f &scale, const Vec2f &center)
	{
		return Vec2f(scale.x * p.x + center.x, scale.y * p.y + center.y);
	}
}

Sprite::Sprite(const unsigned int w, const unsigned int h, const SpriteBatch *p) :
	width(w), height(h), angle(0.0f), stale(-1), vertex_array(nullptr),
	parent(p), pivot(0.5f, 0.5f), scale(1.0f, 1.0f) { }

Sprite::Sprite(const Sprite &s, const SpriteBatch *p) :
	width(s.width), height(s.height), angle(s.angle), stale(-1),
	vertex_array(nullptr), parent(p), center(s.center), pivot(s.pivot),
	position(s.position), scale(s.scale) { }

void Sprite::mirror()
{
	swap(this->vertex_array[0].texcoord, this->vertex_array[1].texcoord);
	swap(this->vertex_array[2].texcoord, this->vertex_array[3].texcoord);
}

void Sprite::move(const Vec2f &delta)
{
	if (delta.is_zero())
		return;

	this->position += delta;
	this->stale |= kStalePosition;
}

void Sprite::rotate(const float r)
{
	if (equal(r, 0.0f))
		return;

	this->angle += r;
	this->stale |= kStaleBuffer;
}

void Sprite::set_color(const unsigned int c)
{
	this->vertex_array[0].color = c;
	this->vertex_array[1].color = c;
	this->vertex_array[2].color = c;
	this->vertex_array[3].color = c;
}

void Sprite::set_pivot(const Vec2f &pivot)
{
	R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f && pivot.y >= 0.0f && pivot.y <= 1.0f,
	         "Invalid pivot point");

	Vec2f diff = pivot;
	diff -= this->pivot;
	if (diff.is_zero())
		return;

	diff.x *= this->width;
	diff.y *= this->height;
	this->center += diff;
	this->position += diff;
	this->pivot = pivot;
}

void Sprite::set_position(const Vec2f &position)
{
	this->position = position;
	this->stale |= kStalePosition;
}

void Sprite::set_rotation(const float r)
{
	this->angle = r;
	this->stale |= kStaleBuffer;
}

void Sprite::set_scale(const float f)
{
	R_ASSERT(f > 0.0f, "Can't scale with a factor of zero or less");

	this->scale.x = f;
	this->scale.y = f;
	this->stale |= kStaleBuffer;
}

void Sprite::set_scale(const Vec2f &f)
{
	R_ASSERT(f.x > 0.0f && f.y > 0.0f, "Can't scale with a factor of zero or less");

	this->scale = f;
	this->stale |= kStaleBuffer;
}

void Sprite::set_texture(const unsigned int id)
{
	const Texture &tx = (*this->parent->texture)[id];
	this->vertex_array[0].texcoord = tx.vx[0];
	this->vertex_array[1].texcoord = tx.vx[1];
	this->vertex_array[2].texcoord = tx.vx[2];
	this->vertex_array[3].texcoord = tx.vx[3];
}

void Sprite::update()
{
	if (!this->stale)
		return;

	if (this->stale & kStaleBuffer)
	{
		if (this->stale & kStalePosition)
			this->center = this->position;

		Vec2f origin[4];
		origin[0].x = this->width * -this->pivot.x;
		origin[0].y = this->height * -(1 - this->pivot.y);
		origin[1].x = origin[0].x + this->width;
		origin[1].y = origin[0].y;
		origin[2].x = origin[1].x;
		origin[2].y = origin[1].y + this->height;
		origin[3].x = origin[0].x;
		origin[3].y = origin[2].y;

		if (!equal(this->angle, 0.0f))
		{
			const float cos_r = cosf(-this->angle);
			const float sin_r = sinf(-this->angle);

			const Vec2f s_sin_r(this->scale.x * sin_r, this->scale.y * sin_r);
			const Vec2f s_cos_r(this->scale.x * cos_r, this->scale.y * cos_r);

			this->vertex_array[0].position = transform_rst(
					origin[0], s_sin_r, s_cos_r, this->center);
			this->vertex_array[1].position = transform_rst(
					origin[1], s_sin_r, s_cos_r, this->center);
			this->vertex_array[2].position = transform_rst(
					origin[2], s_sin_r, s_cos_r, this->center);
			this->vertex_array[3].position = transform_rst(
					origin[3], s_sin_r, s_cos_r, this->center);
		}
		else
		{
			this->vertex_array[0].position = transform_st(
					origin[0], this->scale, this->center);
			this->vertex_array[1].position = transform_st(
					origin[1], this->scale, this->center);
			this->vertex_array[2].position = transform_st(
					origin[2], this->scale, this->center);
			this->vertex_array[3].position = transform_st(
					origin[3], this->scale, this->center);
		}
		this->stale = 0;
	}
	else
	{
		this->position -= this->center;
		this->vertex_array[0].position += this->position;
		this->vertex_array[1].position += this->position;
		this->vertex_array[2].position += this->position;
		this->vertex_array[3].position += this->position;
		this->center += this->position;
		this->position = this->center;
		this->stale = 0;
	}
}
