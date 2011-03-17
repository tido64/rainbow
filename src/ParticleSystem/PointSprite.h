/// A point sprite used in particle systems.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef POINTSPRITE_H_
#define POINTSPRITE_H_

#include "../Types.h"

struct PointSprite
{
	float size;
	Vec2f position;
	Vec2f vector;
	Color4f color;
	Vec2f texcoord[4];
};

#endif
