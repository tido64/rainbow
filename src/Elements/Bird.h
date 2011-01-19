/// Bird is the word.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef BIRD_H_
#define BIRD_H_

#include <Rainbow/Hardware/Screen.h>
#include <Rainbow/Algorithm.h>
#include <Rainbow/Animation.h>

#include "../Objects/Line.h"

class Bird : public Animation
{
public:
	Bird();
	~Bird() { }

	void play();
	void set_sprite(Sprite *) { }
	void set_target(const Vec2 *target);
	void step();
	void update() { }

private:
	float speed;         ///< Traveling speed
	float t;             ///< Timeline indicator

	const Vec2 *target;  ///< The target landing platform

	Vec2f exit;          ///< Exit point
	Vec2f landing;       ///< Landing point
	Vec2f spawn;         ///< Spawning point
};

#endif
