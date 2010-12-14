//
//  OnWireGame.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 7/14/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "OnWireGame.h"

OnWireGame::OnWireGame() :
	time(0), target(time), traveled(target), texture_atlas("assets-hd.png"),
	line(Screen::width(), Screen::height()),
	avatar(this->line.get_displacement_at(4)),
	freetype("/Users/tido/Downloads/frabk.ttf", 20)
{
	// Define background
	this->skyline = this->texture_atlas.create_sprite(skyline_asset);
	this->skyline->scale(0.5f);
	this->skyline->set_position(Screen::width() * 0.5f, Screen::height() * 0.5f);

	// Define building
	Sprite *s = this->texture_atlas.create_sprite(building_asset);
	this->building.set_sprite(s);

	// Define avatar
	s = this->texture_atlas.create_sprite(avatar_asset);
	s->set_texture(0);
	this->avatar.set_sprite(s);

	// Define birds
	//s = this->texture_atlas.create_sprite(yellow_bird_flying);
	//s->set_position(100, 160);

	//s = this->texture_atlas.create_sprite(blue_bird_flying);
	//s->set_position(100, 240);

	//this->elements[0] = new Bird(this->sprite_sheet.get_sprite(yellow_bird_flying));
	//this->elements[1] = new Wind(&this->line);
}

OnWireGame::~OnWireGame()
{
	//delete reinterpret_cast<Bird *>(this->elements[0]);
	//delete reinterpret_cast<Wind *>(this->elements[1]);
}

void OnWireGame::draw()
{
	this->skyline->draw();
	this->building.draw();
	this->line.draw();
	this->avatar.draw();

	this->freetype.test();
}

RealObject **OnWireGame::get_objects()
{
	if (object_count == 0) return 0;

	RealObject **objects = new RealObject *[object_count];
	objects[0] = this->hud.distance->get_label();
	objects[1] = this->hud.distance_label->get_label();
	objects[2] = this->hud.time->get_label();
	objects[3] = this->hud.time_label->get_label();
	return objects;
}

void OnWireGame::reset(const unsigned int t)
{
	this->finished = false;
	this->target = t;
	this->time = 0;
	this->traveled = 0;
	this->hud.reset(this->target);
}

void OnWireGame::tick()
{
	if (this->finished || this->time >= 5999) return;

	this->hud.set_time(++this->time);

	if (Random() < 0.2)
	{
		unsigned int i = static_cast<unsigned int>(Random() * 2);
		this->elements[i]->activate();
	}

	// DEBUGGING ONLY
	//this->travel(1400);
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
	this->hud.set_distance(this->target - this->traveled);
}

void OnWireGame::update()
{
	//this->building.update(this->traveled / this->target);
	this->line.update();
	this->avatar.update();

	/*
	for (unsigned int i = 0; i < element_count; ++i)
	{
		if (!this->elements[i]->active) continue;

		if (this->elements[i]->fire())
			this->elements[i]->active = false;
	}*/

	// Always re-enable client memory before drawing
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
