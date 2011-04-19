#ifndef SPRITEVERTEX_H_
#define SPRITEVERTEX_H_

#include "Vec2.h"

/// A typical sprite vertex contains x- and y-coordinates for position and texture.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
struct SpriteVertex
{
	Vec2f position;
	Vec2f texcoord;
};

#endif
