// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include "Common/Algorithm.h"
#include "Common/Intrinsics.h"
#include "Graphics/SpriteBatch.h"

using Rainbow::equalf;

const unsigned char stale_pivot    = 1u << 0;
const unsigned char stale_position = 1u << 1;
const unsigned char stale_scale    = 1u << 2;
const unsigned char stale_angle    = 1u << 3;

Sprite::Sprite(const unsigned int w, const unsigned int h, const SpriteBatch *p) :
	width(w), height(h), angle(0.0f), stale(-1), vertex_array(nullptr),
	parent(p), pivot(0.5f, 0.5f), scale_f(1.0f, 1.0f) { }

Sprite::Sprite(const Sprite &s, const SpriteBatch *p) :
	width(s.width), height(s.height), angle(s.angle), stale(-1),
	vertex_array(nullptr), parent(p), center(s.center), pivot(s.pivot),
	position(s.position), scale_f(s.scale_f)
{
	memcpy(this->origin, s.origin, sizeof(this->origin));
}

void Sprite::mirror()
{
	Vec2f tmp = this->vertex_array[0].texcoord;
	this->vertex_array[0].texcoord = this->vertex_array[1].texcoord;
	this->vertex_array[1].texcoord = tmp;
	tmp = this->vertex_array[2].texcoord;
	this->vertex_array[2].texcoord = this->vertex_array[3].texcoord;
	this->vertex_array[3].texcoord = tmp;
}

void Sprite::move(const Vec2f &delta)
{
	if (delta.is_zero())
		return;

	this->position += delta;
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
	this->set_color<0>(c);
	this->set_color<1>(c);
	this->set_color<2>(c);
	this->set_color<3>(c);
}

void Sprite::set_pivot(const Vec2f &pivot)
{
	R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f && pivot.y >= 0.0f && pivot.y <= 1.0f,
	         "Invalid pivot point");

	this->pivot = pivot;
	this->stale |= stale_pivot | stale_scale;
}

void Sprite::set_position(const Vec2f &position)
{
	this->position = position;
	this->stale |= stale_position;
}

void Sprite::set_rotation(const float r)
{
	this->angle = r;
	this->stale |= stale_angle | stale_scale;
}

void Sprite::set_scale(const float f)
{
	R_ASSERT(f > 0.0f, "Can't scale with a factor of zero or less");

	this->scale_f.x = f;
	this->scale_f.y = f;
	this->stale |= stale_scale;
}

void Sprite::set_scale(const Vec2f &f)
{
	R_ASSERT(f.x > 0.0f && f.y > 0.0f, "Can't scale with a factor of zero or less");

	this->scale_f = f;
	this->stale |= stale_scale;
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
			this->center = this->position;

		if (!equalf(this->angle, 0.0f))
		{
			const float cos_r = cosf(-this->angle);
			const float sin_r = sinf(-this->angle);

		#if 0 //defined(__SSE3__)

			// sx*cos(r), sx*sin(r), sy*sin(r), sy*cos(r)
			__m128 factor = _mm_set_ps(this->scale_f.y, this->scale_f.y, this->scale_f.x, this->scale_f.x);
			__m128 vx0 = _mm_set_ps(cos_r, sin_r, sin_r, cos_r);
			__m128 pos = _mm_set_ps(this->center.y, this->center.x, this->center.y, this->center.x);
			__m128 vx1, tmp;
			factor = _mm_mul_ps(factor, vx0);

			vx1 = _mm_set_ps(this->origin[0].y, this->origin[0].x, this->origin[0].y, this->origin[0].x);
			vx0 = _mm_mul_ps(factor, vx1);
			vx0 = _mm_set_ps(this->origin[1].y, this->origin[1].x, this->origin[1].y, this->origin[1].x);
			vx1 = _mm_mul_ps(factor, vx0);

			tmp = _mm_shuffle_ps(vx0, vx1, _MM_SHUFFLE(2, 0, 2, 0));
			vx0 = _mm_shuffle_ps(vx0, vx1, _MM_SHUFFLE(3, 1, 3, 1));
			vx1 = _mm_addsub_ps(tmp, vx0);
			vx0 = _mm_add_ps(vx1, pos);

			float result[4];
			_mm_store_ps(result, vx0);
			this->vertex_array[0].position.x = result[0];
			this->vertex_array[0].position.y = result[1];
			this->vertex_array[1].position.x = result[2];
			this->vertex_array[1].position.y = result[3];

			vx1 = _mm_set_ps(this->origin[2].y, this->origin[2].x, this->origin[2].y, this->origin[2].x);
			vx0 = _mm_mul_ps(factor, vx1);
			vx0 = _mm_set_ps(this->origin[3].y, this->origin[3].x, this->origin[3].y, this->origin[3].x);
			vx1 = _mm_mul_ps(factor, vx0);

			tmp = _mm_shuffle_ps(vx0, vx1, _MM_SHUFFLE(2, 0, 2, 0));
			vx0 = _mm_shuffle_ps(vx0, vx1, _MM_SHUFFLE(3, 1, 3, 1));
			vx1 = _mm_addsub_ps(tmp, vx0);
			vx0 = _mm_add_ps(vx1, pos);

			_mm_store_ps(result, vx0);
			this->vertex_array[2].position.x = result[0];
			this->vertex_array[2].position.y = result[1];
			this->vertex_array[3].position.x = result[2];
			this->vertex_array[3].position.y = result[3];

		#else

			const float sx_cos_r = this->scale_f.x * cos_r;
			const float sx_sin_r = this->scale_f.x * sin_r;
			const float sy_sin_r = this->scale_f.y * sin_r;
			const float sy_cos_r = this->scale_f.y * cos_r;

			this->vertex_array[0].position.x = sx_cos_r * this->origin[0].x - sx_sin_r * this->origin[0].y + this->center.x;
			this->vertex_array[0].position.y = sy_sin_r * this->origin[0].x + sy_cos_r * this->origin[0].y + this->center.y;

			this->vertex_array[1].position.x = sx_cos_r * this->origin[1].x - sx_sin_r * this->origin[1].y + this->center.x;
			this->vertex_array[1].position.y = sy_sin_r * this->origin[1].x + sy_cos_r * this->origin[1].y + this->center.y;

			this->vertex_array[2].position.x = sx_cos_r * this->origin[2].x - sx_sin_r * this->origin[2].y + this->center.x;
			this->vertex_array[2].position.y = sy_sin_r * this->origin[2].x + sy_cos_r * this->origin[2].y + this->center.y;

			this->vertex_array[3].position.x = sx_cos_r * this->origin[3].x - sx_sin_r * this->origin[3].y + this->center.x;
			this->vertex_array[3].position.y = sy_sin_r * this->origin[3].x + sy_cos_r * this->origin[3].y + this->center.y;

		#endif
		}
		else
		{
			this->vertex_array[0].position.x = this->scale_f.x * this->origin[0].x + this->center.x;
			this->vertex_array[0].position.y = this->scale_f.y * this->origin[0].y + this->center.y;

			this->vertex_array[1].position.x = this->scale_f.x * this->origin[1].x + this->center.x;
			this->vertex_array[1].position.y = this->scale_f.y * this->origin[1].y + this->center.y;

			this->vertex_array[2].position.x = this->scale_f.x * this->origin[2].x + this->center.x;
			this->vertex_array[2].position.y = this->scale_f.y * this->origin[2].y + this->center.y;

			this->vertex_array[3].position.x = this->scale_f.x * this->origin[3].x + this->center.x;
			this->vertex_array[3].position.y = this->scale_f.y * this->origin[3].y + this->center.y;
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
