/// A point sprite used in particle systems.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef POINTSPRITE_H_
#define POINTSPRITE_H_

#include "../Common/Color.h"
#include "../Common/Vec2.h"

struct PointSprite
{
	float size;
	Vec2f position;
	Vec2f vector;
	Colorf color;
	Vec2f texcoord[4];
};

#endif
