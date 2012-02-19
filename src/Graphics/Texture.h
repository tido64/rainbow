#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Common/Vec2.h"

/// Texture.
///
/// Stores texture id and UV coordinates.
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
	this->vx[0].x = x1; this->vx[0].y = y0;
	this->vx[1].x = x0; this->vx[1].y = y0;
	this->vx[2].x = x1; this->vx[2].y = y1;
	this->vx[3].x = x0; this->vx[3].y = y1;
}

Texture::operator unsigned int() const
{
	return this->atlas;
}

#endif
