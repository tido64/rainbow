/// Bird is the word.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef BIRD_H_
#define BIRD_H_

#include "../Framework/Hardware/Screen.h"
#include "../Framework/Random.h"
#include "../Framework/Animation.h"
#include "../Framework/Element.h"
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
	Random *random;      ///< Random number generator
};

#endif
