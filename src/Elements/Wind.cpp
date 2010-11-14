//
//  Wind.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 10/29/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "Wind.h"

Wind::Wind(Line *l) : force(1.0f), line(l) { }

void Wind::fire()
{
	const unsigned int s = LINE_SEGMENTS >> 1;
	this->line->apply_impulse(Vec2(this->force, 0.0f), s);
	this->line->apply_impulse(Vec2(this->force, 0.0f), s + 1);
	this->line->apply_impulse(Vec2(this->force, 0.0f), s + 2);
}
