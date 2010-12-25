/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef FONTGLYPH_H_
#define FONTGLYPH_H_

#include <Rainbow/Types.h>

struct FontGlyph
{
	int advance;
	SpriteVertex quad[4];
};

#endif
