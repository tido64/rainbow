//
//  Bird.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 11/28/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Bird.h"

using Ars::mt_random;

Bird::Bird() : speed(1.0f / 90.0f) { }

void Bird::activate()
{
	if (this->active) return;

	this->t = 0;
	this->active = true;

	// Random spawning point
	Screen *scr = Screen::Instance();
	if (mt_random() < 0.5f)  // sides
	{
		this->spawn_x = (mt_random() < 0.5f) ? 0.0f : scr->width();
		this->spawn_y = mt_random();
		if (this->spawn_y < 0.5f)
			this->spawn_y += 0.5f;
		this->spawn_y *= scr->height();
	}
	else  // the top
	{
		this->spawn_x = scr->width() * mt_random();
		this->spawn_y = scr->height();
	}

	// Random exit point
	if (mt_random() < 0.5f)  // sides
	{
		this->exit_x = (mt_random() < 0.5f) ? 0.0f : scr->width();
		this->exit_y = mt_random();
		if (this->exit_y < 0.5f)
			this->exit_y += 0.5f;
	}
	else  // top
	{
		this->exit_x = scr->width() * mt_random();
		this->exit_y = scr->height();
	}

	this->landing_x = 120;
	this->landing_y = 160;
}

void Bird::fire()
{
	assert(this->active);

	if (false)  // flying away
	{
		this->active = false;
	}

	if (this->t < 1.0f)  // bird still in air
	{
		float x = Animation::ease_out_quad(this->spawn_x, this->landing_x, this->t);
		float y = Animation::ease_in_cubic(this->spawn_y, this->landing_y, this->t);

		// wing animation?

		this->t += this->speed;
		this->sprite->set_position(x, y);
	}
}

void Bird::set_target(const Vec2 *p)
{
	this->target = p;
	this->sprite->set_position(this->spawn_x, this->spawn_y);
}
