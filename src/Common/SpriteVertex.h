#ifndef SPRITEVERTEX_H_
#define SPRITEVERTEX_H_

#include "Common/Color.h"
#include "Common/Vec2.h"

/// A typical sprite vertex contains x- and y-coordinates for position and texture.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
struct SpriteVertex
{
	static const void *tx_offset;  ///< Offset to texture coordinates
	static const void *vx_offset;  ///< Offset to vertex position 

	Colorb color;    ///< Texture colour, usually white
	Vec2f texcoord;  ///< Texture coordinates
	Vec2f position;  ///< Position of vertex
};

#endif
