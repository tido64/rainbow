/// Bird is the word.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef BIRD_H_
#define BIRD_H_

#include <Rainbow/Hardware/Screen.h>
#include <Rainbow/Animation.h>
#include <Rainbow/ArsMathematica.h>
#include <Rainbow/Element.h>
#include "../Objects/Line.h"

class Bird : public Element
{
public:
	Bird();
	~Bird() { }

	void activate();
	bool fire();
	void set_target(const Vec2 *target);

private:
	const float scr_w;   ///< Width of screen
	const float scr_h;   ///< Height of screen
	const Vec2 *target;  ///< The target landing platform
	float exit_x;        ///< x-component of exit point
	float exit_y;        ///< y-component of exit point
	float landing_x;     ///< x-component of landing point
	float landing_y;     ///< y-component of landing point
	float spawn_x;       ///< x-component of spawning point
	float spawn_y;       ///< y-component of spawning point
	float speed;         ///< Traveling speed
	float t;             ///< Timeline indicator
};

#endif
