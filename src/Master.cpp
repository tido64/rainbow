/*
 *  Master.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Master.h"

Master *Master::Instance()
{
	static Master inst;
	return &inst;
}

void Master::elapse_time()
{
	if (this->finished || this->time >= 5999) return;
	this->hud->update_time(++this->time);

	// DEBUGGING ONLY
	this->travel(2000);
}

float Master::get_progress()
{
	return static_cast<float>(this->traveled) / static_cast<float>(this->target);
}

unsigned int Master::get_time()
{
	return this->time;
}

unsigned int Master::get_traveled()
{
	return this->traveled;
}

void Master::reset(const unsigned int t)
{
	this->finished = false;
	this->target = t;
	this->time = 0;
	this->traveled = 0;
	this->hud->reset(this->target);
}

void Master::travel(const unsigned int d)
{
	if (this->traveled == this->target) return;

	this->traveled += d;
	if (this->traveled >= this->target)
	{
		this->traveled = this->target;
		this->finished = true;
	}
	this->hud->update_distance(this->target - this->traveled);
}

void Master::use(HUD *hud)
{
	this->hud = hud;
}
