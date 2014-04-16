// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

using Rainbow::equal;

namespace
{
	const unsigned int kStaleBuffer       = 1u << 0;
	const unsigned int kStalePosition     = 1u << 1;
	const unsigned int kStaleFrontBuffer  = 1u << 2;

	inline Vec2f transform_rst(const Vec2f &p,
	                           const Vec2f &s_sin_r,
	                           const Vec2f &s_cos_r,
	                           const Vec2f &center) pure;
	inline Vec2f transform_st(const Vec2f &p,
	                          const Vec2f &scale,
	                          const Vec2f &center) pure;

	Vec2f transform_rst(const Vec2f &p,
	                    const Vec2f &s_sin_r,
	                    const Vec2f &s_cos_r,
	                    const Vec2f &center)
	{
		return Vec2f(s_cos_r.x * p.x - s_sin_r.x * p.y + center.x,
		             s_sin_r.y * p.x + s_cos_r.y * p.y + center.y);
	}

	Vec2f transform_st(const Vec2f &p, const Vec2f &scale, const Vec2f &center)
	{
		return Vec2f(scale.x * p.x + center.x, scale.y * p.y + center.y);
	}
}

Sprite::Ref::Ref(const SpriteBatch *batch, const size_t i)
    : batch(batch), i(i) { }

Sprite& Sprite::Ref::operator*() const
{
	return this->batch->sprites()[this->i];
}

Sprite* Sprite::Ref::operator->() const
{
	return &this->batch->sprites()[this->i];
}

Sprite::Sprite(const unsigned int w, const unsigned int h, const SpriteBatch *p)
    : angle_(0.0f), width_(w), height_(h), stale_(-1), vertex_array_(nullptr),
      parent_(p), pivot_(0.5f, 0.5f), scale_(1.0f, 1.0f) { }

Sprite::Sprite(Sprite &&s)
    : angle_(s.angle_), width_(s.width_), height_(s.height_), stale_(s.stale_),
      vertex_array_(s.vertex_array_), parent_(s.parent_), center_(s.center_),
      pivot_(s.pivot_), position_(s.position_), scale_(s.scale_)
{
	s.vertex_array_ = nullptr;
	s.parent_ = nullptr;
}

void Sprite::set_color(const unsigned int c)
{
	R_ASSERT(this->vertex_array_, "Missing vertex array buffer");

	this->vertex_array_[0].color = c;
	this->vertex_array_[1].color = c;
	this->vertex_array_[2].color = c;
	this->vertex_array_[3].color = c;
	this->stale_ |= kStaleFrontBuffer;
}

void Sprite::set_normal(const unsigned int id)
{
	R_ASSERT(this->normal_map_, "Missing normal map buffer");

	const Texture &normal = this->parent_->normal()[id];
	this->normal_map_[0] = normal.vx[0];
	this->normal_map_[1] = normal.vx[1];
	this->normal_map_[2] = normal.vx[2];
	this->normal_map_[3] = normal.vx[3];
	this->stale_ |= kStaleFrontBuffer;
}

void Sprite::set_pivot(const Vec2f &pivot)
{
	R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f &&
	         pivot.y >= 0.0f && pivot.y <= 1.0f,
	         "Invalid pivot point");

	Vec2f diff = pivot;
	diff -= this->pivot_;
	if (diff.is_zero())
		return;

	diff.x *= this->width_ * this->scale_.x;
	diff.y *= this->height_ * this->scale_.y;
	this->center_ += diff;
	this->position_ += diff;
	this->pivot_ = pivot;
}

void Sprite::set_position(const Vec2f &position)
{
	this->position_ = position;
	this->stale_ |= kStalePosition;
}

void Sprite::set_rotation(const float r)
{
	this->angle_ = r;
	this->stale_ |= kStaleBuffer;
}

void Sprite::set_scale(const float f)
{
	this->set_scale(Vec2f(f, f));
}

void Sprite::set_scale(const Vec2f &f)
{
	R_ASSERT(f.x > 0.0f && f.y > 0.0f,
	         "Can't scale with a factor of zero or less");

	this->scale_ = f;
	this->stale_ |= kStaleBuffer;
}

void Sprite::set_texture(const unsigned int id)
{
	const Texture &tx = this->parent_->texture()[id];
	this->vertex_array_[0].texcoord = tx.vx[0];
	this->vertex_array_[1].texcoord = tx.vx[1];
	this->vertex_array_[2].texcoord = tx.vx[2];
	this->vertex_array_[3].texcoord = tx.vx[3];
	this->stale_ |= kStaleFrontBuffer;
}

void Sprite::mirror()
{
	std::swap(this->vertex_array_[0].texcoord, this->vertex_array_[1].texcoord);
	std::swap(this->vertex_array_[2].texcoord, this->vertex_array_[3].texcoord);
}

void Sprite::move(const Vec2f &delta)
{
	if (delta.is_zero())
		return;

	this->position_ += delta;
	this->stale_ |= kStalePosition;
}

void Sprite::rotate(const float r)
{
	if (equal(r, 0.0f))
		return;

	this->angle_ += r;
	this->stale_ |= kStaleBuffer;
}

bool Sprite::update()
{
	if (!this->stale_)
		return false;

	R_ASSERT(this->vertex_array_, "Missing vertex array buffer");

	if (this->stale_ & kStaleBuffer)
	{
		if (this->stale_ & kStalePosition)
			this->center_ = this->position_;

		Vec2f origin[4];
		origin[0].x = this->width_ * -this->pivot_.x;
		origin[0].y = this->height_ * -(1 - this->pivot_.y);
		origin[1].x = origin[0].x + this->width_;
		origin[1].y = origin[0].y;
		origin[2].x = origin[1].x;
		origin[2].y = origin[1].y + this->height_;
		origin[3].x = origin[0].x;
		origin[3].y = origin[2].y;

		if (!equal(this->angle_, 0.0f))
		{
			const float cos_r = cosf(-this->angle_);
			const float sin_r = sinf(-this->angle_);

			const Vec2f s_sin_r(this->scale_.x * sin_r, this->scale_.y * sin_r);
			const Vec2f s_cos_r(this->scale_.x * cos_r, this->scale_.y * cos_r);

			this->vertex_array_[0].position =
			    transform_rst(origin[0], s_sin_r, s_cos_r, this->center_);
			this->vertex_array_[1].position =
			    transform_rst(origin[1], s_sin_r, s_cos_r, this->center_);
			this->vertex_array_[2].position =
			    transform_rst(origin[2], s_sin_r, s_cos_r, this->center_);
			this->vertex_array_[3].position =
			    transform_rst(origin[3], s_sin_r, s_cos_r, this->center_);
		}
		else
		{
			this->vertex_array_[0].position =
			    transform_st(origin[0], this->scale_, this->center_);
			this->vertex_array_[1].position =
			    transform_st(origin[1], this->scale_, this->center_);
			this->vertex_array_[2].position =
			    transform_st(origin[2], this->scale_, this->center_);
			this->vertex_array_[3].position =
			    transform_st(origin[3], this->scale_, this->center_);
		}
	}
	else if (this->stale_ & kStalePosition)
	{
		this->position_ -= this->center_;
		this->vertex_array_[0].position += this->position_;
		this->vertex_array_[1].position += this->position_;
		this->vertex_array_[2].position += this->position_;
		this->vertex_array_[3].position += this->position_;
		this->center_ += this->position_;
		this->position_ = this->center_;
	}
	this->stale_ = 0;
	return true;
}
