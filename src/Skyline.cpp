/*
 *  Skyline.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 6/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Skyline.h"

Skyline::Skyline(Screen &s) : Sprite("OnWire_thirdPerson01.png")
{
	int s_w = s.get_width();
	int s_h = s.get_height();
	this->scale(s_w / 600.0f, s_h / 763.0f);
	this->set_position(s_w >> 1, s_h >> 1);
}
