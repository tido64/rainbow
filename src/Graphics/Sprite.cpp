// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

using Rainbow::isequal;

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
    : batch_(batch), i_(i) { }

Sprite& Sprite::Ref::operator*() const
{
	return batch_->sprites()[i_];
}

Sprite* Sprite::Ref::operator->() const
{
	return &batch_->sprites()[i_];
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
	R_ASSERT(vertex_array_, "Missing vertex array buffer");

	vertex_array_[0].color = c;
	vertex_array_[1].color = c;
	vertex_array_[2].color = c;
	vertex_array_[3].color = c;
	stale_ |= kStaleFrontBuffer;
}

void Sprite::set_normal(const unsigned int id)
{
	R_ASSERT(normal_map_, "Missing normal map buffer");

	const Texture &normal = parent_->normal()[id];
	normal_map_[0] = normal.vx[0];
	normal_map_[1] = normal.vx[1];
	normal_map_[2] = normal.vx[2];
	normal_map_[3] = normal.vx[3];
	stale_ |= kStaleFrontBuffer;
}

void Sprite::set_pivot(const Vec2f &pivot)
{
	R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f &&
	         pivot.y >= 0.0f && pivot.y <= 1.0f,
	         "Invalid pivot point");

	Vec2f diff = pivot;
	diff -= pivot_;
	if (diff.is_zero())
		return;

	diff.x *= width_ * scale_.x;
	diff.y *= height_ * scale_.y;
	center_ += diff;
	position_ += diff;
	pivot_ = pivot;
}

void Sprite::set_position(const Vec2f &position)
{
	position_ = position;
	stale_ |= kStalePosition;
}

void Sprite::set_rotation(const float r)
{
	angle_ = r;
	stale_ |= kStaleBuffer;
}

void Sprite::set_scale(const float f)
{
	set_scale(Vec2f(f, f));
}

void Sprite::set_scale(const Vec2f &f)
{
	R_ASSERT(f.x > 0.0f && f.y > 0.0f,
	         "Can't scale with a factor of zero or less");

	scale_ = f;
	stale_ |= kStaleBuffer;
}

void Sprite::set_texture(const unsigned int id)
{
	const Texture &tx = parent_->texture()[id];
	vertex_array_[0].texcoord = tx.vx[0];
	vertex_array_[1].texcoord = tx.vx[1];
	vertex_array_[2].texcoord = tx.vx[2];
	vertex_array_[3].texcoord = tx.vx[3];
	stale_ |= kStaleFrontBuffer;
}

void Sprite::mirror()
{
	std::swap(vertex_array_[0].texcoord, vertex_array_[1].texcoord);
	std::swap(vertex_array_[2].texcoord, vertex_array_[3].texcoord);
}

void Sprite::move(const Vec2f &delta)
{
	if (delta.is_zero())
		return;

	position_ += delta;
	stale_ |= kStalePosition;
}

void Sprite::rotate(const float r)
{
	if (isequal(r, 0.0f))
		return;

	angle_ += r;
	stale_ |= kStaleBuffer;
}

bool Sprite::update()
{
	if (!stale_)
		return false;

	R_ASSERT(vertex_array_, "Missing vertex array buffer");

	if (stale_ & kStaleBuffer)
	{
		if (stale_ & kStalePosition)
			center_ = position_;

		Vec2f origin[4];
		origin[0].x = width_ * -pivot_.x;
		origin[0].y = height_ * -(1 - pivot_.y);
		origin[1].x = origin[0].x + width_;
		origin[1].y = origin[0].y;
		origin[2].x = origin[1].x;
		origin[2].y = origin[1].y + height_;
		origin[3].x = origin[0].x;
		origin[3].y = origin[2].y;

		if (!isequal(angle_, 0.0f))
		{
			const float cos_r = cosf(-angle_);
			const float sin_r = sinf(-angle_);

			const Vec2f s_sin_r(scale_.x * sin_r, scale_.y * sin_r);
			const Vec2f s_cos_r(scale_.x * cos_r, scale_.y * cos_r);

			vertex_array_[0].position =
			    transform_rst(origin[0], s_sin_r, s_cos_r, center_);
			vertex_array_[1].position =
			    transform_rst(origin[1], s_sin_r, s_cos_r, center_);
			vertex_array_[2].position =
			    transform_rst(origin[2], s_sin_r, s_cos_r, center_);
			vertex_array_[3].position =
			    transform_rst(origin[3], s_sin_r, s_cos_r, center_);
		}
		else
		{
			vertex_array_[0].position =
			    transform_st(origin[0], scale_, center_);
			vertex_array_[1].position =
			    transform_st(origin[1], scale_, center_);
			vertex_array_[2].position =
			    transform_st(origin[2], scale_, center_);
			vertex_array_[3].position =
			    transform_st(origin[3], scale_, center_);
		}
	}
	else if (stale_ & kStalePosition)
	{
		position_ -= center_;
		vertex_array_[0].position += position_;
		vertex_array_[1].position += position_;
		vertex_array_[2].position += position_;
		vertex_array_[3].position += position_;
		center_ += position_;
		position_ = center_;
	}
	stale_ = 0;
	return true;
}
