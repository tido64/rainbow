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
	this->sprite->set_position(Screen::Instance().width() * 0.5f, Screen::Instance().height() * 0.5f);
}

void Building::touch_began(const Touches &touches)
{
	this->pinch();
}

void Building::touch_canceled()
{
	this->angle += this->pinch.angle;
	this->scale *= this->pinch.scale;
	this->pinch();
}

void Building::touch_ended(const Touches &touches)
{
	this->angle += this->pinch.angle;
	this->scale *= this->pinch.scale;
	this->pinch();
}

void Building::touch_moved(const Touches &touches)
{
	this->pinch();
	this->sprite->rotate(this->angle + this->pinch.angle);
	this->sprite->scale(this->scale * this->pinch.scale);
}

void Building::update()
{
	assert(this->sprite != 0);

	return;

	float p = OnWireState::Instance().traveled / OnWireState::Instance().target;
	if (p == this->progress) return;

	this->progress = p;
	this->sprite->scale(this->scale + this->progress * 0.5f);
}
