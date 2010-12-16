//
//  Bird.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 11/28/10.
//  Copyright 2010 Ninja Unicorn. All rights reserved.
//

#include "Bird.h"

Bird::Bird() :
	scr_w(Screen::width()), scr_h(Screen::height()), speed(1.0f / 90.0f)
{ }

void Bird::activate()
{
	if (this->active) return;

	this->t = 0;
	this->active = true;

	// Random spawning point
	if (Random() < 0.5f)  // sides
	{
		this->spawn_x = (Random() < 0.5f) ? 0.0f : scr_w;
		this->spawn_y = Random();
		if (this->spawn_y < 0.5f)
			this->spawn_y += 0.5f;
		this->spawn_y *= scr_h;
	}
	else  // the top
	{
		this->spawn_x = scr_w * Random();
		this->spawn_y = scr_h;
	}

	// Random exit point
	if (Random() < 0.5f)  // sides
	{
		this->exit_x = (Random() < 0.5f) ? 0.0f : scr_w;
		this->exit_y = Random();
		if (this->exit_y < 0.5f)
			this->exit_y += 0.5f;
	}
	else  // top
	{
		this->exit_x = scr_w * Random();
		this->exit_y = scr_h;
	}

	this->landing_x = 120;
	this->landing_y = 160;
}

bool Bird::fire()
{
	assert(this->active);

	if (false)  // flying away
	{
		return true;
	}

	if (this->t < 1.0f)  // bird still in air
	{
		float x = Animation::ease_out_quad(this->spawn_x, this->landing_x, this->t);
		float y = Animation::ease_in_cubic(this->spawn_y, this->landing_y, this->t);

		// wing animation?

		this->t += this->speed;
		this->sprite->set_position(x, y);
	}
	return false;
}

void Bird::set_target(const Vec2 *p)
{
	this->target = p;
	if (Random() < 0.5f)
	{
		this->exit_x = 0.0f;
		this->spawn_x = this->scr_w;
	}
	else
	{
		this->exit_x = this->scr_w;
		this->spawn_x = 0.0f;
	}
	this->sprite->set_position(this->spawn_x, this->spawn_y);
}
