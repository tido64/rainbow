/*
 *  Skyline.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 6/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Skyline.h"

Skyline::Skyline() : Sprite("Skyline.png")
{
	Screen *s = Screen::Instance();
	int s_w = s->get_width();
	int s_h = s->get_height();
	this->scale(s_w / 643.0f, s_h / 962.0f);
	this->set_position(s_w >> 1, s_h >> 1);
}
