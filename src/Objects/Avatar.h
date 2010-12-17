/// This is the representation of the player.

/// Copyright 2010 Ninja Unicorn. All rights reserved.
/// \author Tommy Nguyen

#ifndef AVATAR_H_
#define AVATAR_H_

#include <cmath>

#include "../Framework/Hardware/Screen.h"
#include "../Framework/Drawable.h"
#include "../Framework/Physics.h"

class Avatar : public Drawable
{
public:
	Avatar(const Vec2 *base);

	void set_sprite(Sprite *s);
	void update();

private:
	const float head_x, head_y;
	const Vec2 *base;
};

#endif
