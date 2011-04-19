/// Font glyph structure for storing advance and a textured sprite.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef FONTGLYPH_H_
#define FONTGLYPH_H_

#include "Common/SpriteVertex.h"

struct FontGlyph
{
	unsigned short int advance;
	short int left;
	short int kern[95];
	SpriteVertex quad[4];
};

#endif
