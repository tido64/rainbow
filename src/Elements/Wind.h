/// Wind game element affects the behaviour of the line.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef WIND_H_
#define WIND_H_

#include <Rainbow/Animation.h>

#include "../Objects/Line.h"

class Wind : public Animation
{
public:
	Wind(const Line *);
	void play();
	void set_sprite(Sprite *) { }
	void step();
	void update() { }

private:
	float force;
	const Line *line;
};

#endif
