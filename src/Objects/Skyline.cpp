//
//  Skyline.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "Skyline.h"

Skyline::Skyline(Sprite *s) :
	sprite(s)
{
	float s_w = Screen::width();
	float s_h = Screen::height();
	this->sprite->scale(s_w / this->sprite->raw_width, s_h / this->sprite->raw_height);
	this->sprite->set_position(s_w * 0.5f, s_h * 0.5f);
}
