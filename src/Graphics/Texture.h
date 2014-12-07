// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTURE_H_
#define GRAPHICS_TEXTURE_H_

#include "Common/Vec2.h"

/// Stores texture id and UV coordinates.
///
/// 3 ┌─────┐ 2
///   │     │
///   │     │
/// 0 └─────┘ 1
///
/// Textures are read into memory upside-down. Therefore, the order of the UV
/// coordinates are flipped vertically, giving us 3,2,1 and 1,0,3.
struct Texture
{
	Vec2f vx[4];
	unsigned int atlas;

	inline Texture();
	inline Texture(const Vec2f &v0, const Vec2f &v1);

	inline operator unsigned int() const;
};

Texture::Texture() : atlas(0) {}

Texture::Texture(const Vec2f &v0, const Vec2f &v1) : atlas(0)
{
	this->vx[0].x = v0.x; this->vx[0].y = v1.y;
	this->vx[1].x = v1.x; this->vx[1].y = v1.y;
	this->vx[2].x = v1.x; this->vx[2].y = v0.y;
	this->vx[3].x = v0.x; this->vx[3].y = v0.y;
}

Texture::operator unsigned int() const
{
	return this->atlas;
}

#endif
