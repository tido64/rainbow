#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Common/Vec2.h"

/// Texture.
///
/// Stores texture id and UV coordinates.
///
/// 3 ┌─────┐ 2
///   │     │
///   │     │
/// 0 └─────┘ 1
///
/// Textures are read into memory upside-down. Therefore, the order of the UV
/// coordinates are flipped vertically, giving us 3,2,1 and 1,0,3.
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
struct Texture
{
	unsigned int atlas;
	Vec2f vx[4];

	inline Texture();
	inline Texture(const float x0, const float y0, const float x1, const float y1);

	inline operator unsigned int() const;
};

Texture::Texture() : atlas(0) { }

Texture::Texture(const float x0, const float y0, const float x1, const float y1) :
	atlas(0)
{
	this->vx[0].x = x0; this->vx[0].y = y1;
	this->vx[1].x = x1; this->vx[1].y = y1;
	this->vx[2].x = x1; this->vx[2].y = y0;
	this->vx[3].x = x0; this->vx[3].y = y0;
}

Texture::operator unsigned int() const
{
	return this->atlas;
}

#endif
