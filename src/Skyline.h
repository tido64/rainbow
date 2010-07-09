/*
 *  Skyline.h
 *  OnWire
 *
 *	Skyline sprite. Created and properly positioned on the screen.
 *
 *  Created by Tommy Nguyen on 6/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SKYLINE_H_
#define SKYLINE_H_

#include "Screen.h"
#include "Sprite.h"

class Skyline : public Sprite
{
public:
	Skyline(Screen &screen);
	~Skyline() { }
	void update() { }
};

#endif
