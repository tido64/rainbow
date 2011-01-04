//
//  Building.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 7/14/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Building.h"

Building::Building() :
	scale(0.4f), progress(scale)
{ }

void Building::set_sprite(Sprite *s)
{
	this->sprite = s;
	this->sprite->scale(this->scale);
	this->sprite->set_pivot(0.5f, 0.39f);
	this->sprite->set_position(Screen::Instance()->width() * 0.5f, Screen::Instance()->height() * 0.5f);
}

void Building::update(const float &p)
{
	assert(this->sprite != 0);

	this->scale += 0.005;               //DEBUG
	this->sprite->rotate(this->scale);  //DEBUG
	return;                             //DEBUG

	if (p == this->progress) return;

	this->progress = p;
	this->sprite->scale(this->scale + this->progress * 0.5f);
}
