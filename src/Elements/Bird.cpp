//
//  Bird.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 11/28/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Bird.h"

using Rainbow::mt_random;

Bird::Bird() : speed(1.0f / 90.0f) { }

void Bird::play()
{
	if (!this->stopped) return;

	this->t = 0;
	this->stopped = false;

	// Random spawning point
	if (mt_random() < 0.5f)  // sides
	{
		this->spawn.x = (mt_random() < 0.5f) ? 0.0f : Screen::Instance().width();
		this->spawn.y = mt_random();
		if (this->spawn.y < 0.5f)
			this->spawn.y += 0.5f;
		this->spawn.y *= Screen::Instance().height();
	}
	else  // the top
	{
		this->spawn.x = Screen::Instance().width() * mt_random();
		this->spawn.y = Screen::Instance().height();
	}

	// Random exit point
	if (mt_random() < 0.5f)  // sides
	{
		this->exit.x = (mt_random() < 0.5f) ? 0.0f : Screen::Instance().width();
		this->exit.y = mt_random();
		if (this->exit.y < 0.5f)
			this->exit.y += 0.5f;
	}
	else  // top
	{
		this->exit.x = Screen::Instance().width() * mt_random();
		this->exit.y = Screen::Instance().height();
	}

	this->landing.x = 120;
	this->landing.y = 160;
}

void Bird::set_target(const Vec2 *p)
{
	this->target = p;
	this->sprite->set_position(this->spawn);
}

void Bird::step()
{
	assert(!this->stopped);

	if (false)  // flying away
	{
		this->stopped = true;
	}

	if (this->t < 1.0f)  // bird still in air
	{
		float x = ease_out_quad(this->spawn.x, this->landing.x, this->t);
		float y = ease_in_cubic(this->spawn.y, this->landing.y, this->t);

		// wing animation?

		this->t += this->speed;
		this->sprite->set_position(x, y);
	}
}
