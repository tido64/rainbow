#ifndef FONTGLYPH_H_
#define FONTGLYPH_H_

#include "Graphics/SpriteVertex.h"

/// Font glyph structure for storing advance and a textured sprite.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
struct FontGlyph
{
	unsigned short int advance;  ///< Horizontal advancement.
	short int left;              ///< Left alignment.
	unsigned long code;          ///< UTF-32 code.
	SpriteVertex quad[4];        ///< Sprite vertices.

#ifdef FONTATLAS_KERNING
	short int kern[95];          ///< Font kerning information for each character.
#endif
};

#endif
