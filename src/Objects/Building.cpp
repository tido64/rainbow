//
//  Building.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "Building.h"

Building::Building() :
	scale(0.5f), progress(scale)
{ }

void Building::set_sprite(Sprite *s)
{
	this->sprite = s;
	this->sprite->scale(this->scale);
	this->sprite->set_pivot(this->scale, 0.39f);
	this->sprite->set_position(Screen::width() * this->scale, Screen::height() * this->scale);
}

void Building::update(const float &p)
{
	assert(this->sprite != 0);

	this->scale += 0.001f;
	this->sprite->scale(this->scale);
	return;


	if (p == this->progress) return;

	this->progress = p;
	this->sprite->scale(this->scale + this->progress * 0.5f);
}
