//
//  Skyline.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "Skyline.h"

Skyline::Skyline() :
	Sprite("Skyline.png")
{
	float s_w = Screen::width();
	float s_h = Screen::height();
	this->scale(s_w / 643.0f, s_h / 962.0f);
	this->set_position(s_w * 0.5f, s_h * 0.5f);
}
