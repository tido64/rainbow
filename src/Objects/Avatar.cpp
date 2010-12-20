//
//  Avatar.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 11/17/10.
//  Copyright 2010 Ninja Unicorn. All rights reserved.
//

#include "Avatar.h"

Avatar::Avatar(const Vec2 *b) :
	head_x(b->x), head_y(Screen::height() * 0.5f / PTM_RATIO), base(b)
{ }

void Avatar::set_sprite(Sprite *s)
{
	this->sprite = s;
	this->sprite->scale(0.5f);
	this->sprite->set_pivot(0.5f, 0.0f);
	this->sprite->set_position(this->base->x * PTM_RATIO, this->base->y * PTM_RATIO);
}

void Avatar::update()
{
	this->sprite->set_position(this->base->x * PTM_RATIO, this->base->y * PTM_RATIO);
	this->sprite->rotate(atan((this->head_x - this->base->x) / this->head_y));
	//this->sprite->update();
}
