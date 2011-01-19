//
//  OnWireGame.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 7/14/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "OnWireGame.h"

using Rainbow::mt_random;

OnWireGame::OnWireGame() :
	texture_atlas("assets-hd.png", 11, 8),
	line(Screen::Instance().width(), Screen::Instance().height()),
	avatar(this->line.get_displacement_at(4)),
	wind(&this->line)
{
	// Define background
	this->skyline = this->texture_atlas.create_sprite(skyline_asset);
	this->skyline->scale(0.5f);
	this->skyline->set_position(Screen::Instance().width() * 0.5f, Screen::Instance().height() * 0.5f);
	this->background.add(this->skyline);

	// Define building
	Controls::Instance()->add_touchable(&this->building);
	Sprite *s = this->texture_atlas.create_sprite(building_asset);
	this->building.set_sprite(s);
	this->background.add(s);

	// Update background sprites
	this->background.update();

	// Define avatar
	s = this->texture_atlas.create_sprite(avatar_asset);
	s->set_texture(0);
	this->avatar.set_sprite(s);
	this->foreground.add(s);

	// Update foreground sprites
	this->foreground.update();

	// Define birds
	//s = this->texture_atlas.create_sprite(yellow_bird_flying);
	//s->set_position(100, 160);

	//s = this->texture_atlas.create_sprite(blue_bird_flying);
	//s->set_position(100, 240);

	this->yellow_bird.set_sprite(this->texture_atlas.create_sprite(yellow_bird_flying));
	this->clips[0] = &this->yellow_bird;
	this->clips[1] = &this->wind;

	this->rain.emit(100);
	this->rain.update();
	/*
	SpriteVertex svx;
	const void *tex_offset = &svx.position;
	const void *tes_offset = &svx.texcoord;
	const unsigned int ptr = reinterpret_cast<unsigned int>(tes_offset) - reinterpret_cast<unsigned int> (tex_offset);
	CCLOG(@"Pointers: %u", ptr);
	CCLOG(@"Pointer: %u", reinterpret_cast<float *>(0) + 2);
	*/

	AssetManager::Instance()->close();
}

void OnWireGame::draw()
{
	this->background.draw();
	this->line.draw();
	this->foreground.draw();
	this->rain.draw();
	this->hud.draw();
}

void OnWireGame::reset(const unsigned int t)
{
	OnWireState::Instance().reset(t);
	this->hud.reset(t);
}

void OnWireGame::tick()
{
	if (OnWireState::Instance().finished) return;

	if (OnWireState::Instance().time < 5999)
		this->hud.set_time(++OnWireState::Instance().time);

	this->clips[1]->play();  // DEBUG
	return;                  // DEBUG

	if (mt_random() < 0.2)
	{
		unsigned int i = static_cast<unsigned int>(mt_random() * 2);
		this->clips[i]->play();
	}
}

void OnWireGame::travel(const unsigned int d)
{
	if (OnWireState::Instance().traveled == OnWireState::Instance().target) return;

	OnWireState::Instance().traveled += d;
	if (OnWireState::Instance().traveled >= OnWireState::Instance().target)
	{
		OnWireState::Instance().traveled = OnWireState::Instance().target;
		OnWireState::Instance().finished = true;
	}
	this->hud.set_distance(OnWireState::Instance().target - OnWireState::Instance().traveled);
}

void OnWireGame::update()
{
	// Update game elements
	this->building.update();
	this->line.update();
	this->avatar.update();

	// Update game obstacles
	for (unsigned int i = 0; i < clip_count; ++i)
	{
		if (this->clips[i]->stopped) continue;

		this->clips[i]->step();
	}

	// Update sprites
	this->background.update();
	this->foreground.update();
	this->rain.update();
	this->hud.update();
}
