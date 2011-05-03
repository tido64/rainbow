#ifndef SPRITEVERTEX_H_
#define SPRITEVERTEX_H_

#include "Color.h"
#include "Vec2.h"

/// A typical sprite vertex contains x- and y-coordinates for position and texture.

/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
struct SpriteVertex
{
	Colorb color;
	Vec2f texcoord;
	Vec2f position;
};

#endif
