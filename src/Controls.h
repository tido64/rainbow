/*
 *  Controls.h
 *  OnWire
 *
 *	Handles accelerometer/gyroscope and touch events independent of platform.
 *
 *  Created by Tommy Nguyen on 6/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "Sprite.h"

#if defined(ONWIRE_ANDROID)
#elif defined(ONWIRE_IOS)
#	include "CCTouchDispatcher.h"
#endif

class Controls
{
public:
	Controls(const unsigned int w, const unsigned int h);
	void accelerate(const float x, const float y);
	void add_object(Sprite *);
	void touch();

private:
	const unsigned int scr_width, scr_height, speed;
	unsigned int sprite_count;
	Sprite *sprites;
};

#endif CONTROLS_H_
