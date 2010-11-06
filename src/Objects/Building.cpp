/*
 *  Building.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Building.h"

Building::Building() :
	Sprite("Building.png"), initial(0.3f), progress(initial)
{
	Screen *s = Screen::Instance();
	//this->scale(s_w / 488, s_h / 1547.0f);
	this->scale(this->initial);
	this->set_position(s->get_width() >> 1, 200);
}

void Building::update()
{
	const float p = Master::Instance()->get_progress();
	if (p == this->progress) return;
	this->progress = p;

	this->scale(this->initial + this->progress * 0.5f);
}
