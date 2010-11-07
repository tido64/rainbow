/*
 *  OnWireGame.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "OnWireGame.h"

OnWireGame::OnWireGame() :
	time(0), target(time), traveled(target),
	building(new Building()),
	hud(new HUD()),
	line(new Line(Screen::Instance()->get_height() * 0.5f)),
	skyline(new Skyline())
{
}

OnWireGame::~OnWireGame()
{
	delete this->building;
	delete this->hud;
	delete this->line;
	delete this->skyline;
}

void OnWireGame::draw()
{
	this->line->draw();
}

void OnWireGame::elapse_time()
{
	if (this->finished || this->time >= 5999) return;
	this->hud->update_time(++this->time);

	// DEBUGGING ONLY
	this->travel(1400);
	this->play();
}

RealSprite **OnWireGame::get_game_objects()
{
	RealSprite **sprites = new RealSprite *[OBJECT_COUNT];
	sprites[0] = this->skyline->get_object();
	sprites[1] = this->building->get_object();
	return sprites;
}

void OnWireGame::play()
{
	// apply pranks and kinds of shit here
	WindAction wa(this->line);
	wa.fire();
}

void OnWireGame::reset(const unsigned int t)
{
	this->finished = false;
	this->target = t;
	this->time = 0;
	this->traveled = 0;
	this->hud->reset(this->target);
}

void OnWireGame::travel(const unsigned int d)
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


void OnWireGame::update()
{
	this->building->update(this->traveled / this->target);
	this->line->update();
}
