/// The target building.

/// This is where our player wants to reach. Zooms in as the player gets closer
/// to the building.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef BUILDING_H_
#define BUILDING_H_

#include <Rainbow/Hardware/Screen.h>
#include <Rainbow/Drawable.h>

class Building : public Drawable
{
public:
	Building();

	void set_sprite(Sprite *);
	void update(const float &progress);

private:
	float scale, progress;
};

#endif
