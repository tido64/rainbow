//
//  Avatar.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 11/17/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Avatar.h"

Avatar::Avatar(const Vec2 *b) :
	base(b), head(b->x, static_cast<float>(Screen::Instance().height() * 0.5f) / PTM_RATIO)
{ }

void Avatar::set_sprite(Sprite *s)
{
	this->sprite = s;
	//this->sprite->scale(0.5f);
	this->sprite->set_pivot(0.5f, 0.0f);
	this->sprite->set_position(this->base->x * PTM_RATIO, this->base->y * PTM_RATIO);
}

void Avatar::update()
{
	this->sprite->set_position(this->base->x * PTM_RATIO, this->base->y * PTM_RATIO);
	this->sprite->rotate(atanf((this->head.x - this->base->x) / this->head.y));
}
