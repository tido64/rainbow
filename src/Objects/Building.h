/// The target building.

/// This is where our player wants to reach. Zooms in as the player gets closer
/// to the building.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef BUILDING_H_
#define BUILDING_H_

#include <Rainbow/Hardware/Screen.h>
#include <Rainbow/Input/Gestures.h>
#include <Rainbow/Algorithm.h>
#include <Rainbow/Drawable.h>
#include <Rainbow/Touchable.h>

#include "../OnWireState.h"

class Building : public Drawable, public Touchable
{
public:
	Building();

	void set_sprite(Sprite *);
	void touch_began(const Touches &touches);
	void touch_canceled();
	void touch_ended(const Touches &touches);
	void touch_moved(const Touches &touches);
	void update();

private:
	float angle, scale, progress;
	Rainbow::Gestures::Pinch pinch;
};

#endif
