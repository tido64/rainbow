/*
 *  Director.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 2/13/11.
 *  Copyright 2011 Bifrost Games. All rights reserved.
 *
 */

#include "Director.h"

using Rainbow::ConFuoco::Mixer;

Director::Director()
{
#if defined(RAINBOW_IOS)

	// Asset source set in Lua constructor
	this->update_video();

#elif defined(RAINBOW_ANDROID)

#endif

	// Add director to input event loop
	Input::Instance().subscribe(this, -1);
}

void Director::draw()
{
	/*if (this->drawables.size() == 0)
		return

	Drawable **obj = this->drawables.begin();
	for (unsigned int i = 0; i < this->drawables.size(); ++i)
	{
		(*obj)->draw();
		++obj;
	}*/
	this->lua.call("draw");
}

void Director::init(const char *script)
{
	this->lua.load(AssetManager::Instance().get_full_path(script));
	this->lua.call("init");
	this->lua.call("update");
}

void Director::update(const float dt)
{
	Mixer::Instance().update();
	Physics::Instance().step(dt);

	#ifdef RAINBOW_ACCELERATED
		this->lua.input.update(this->lua.L);
	#endif
	this->lua.call("update");

	/*Drawable **obj = this->drawables.begin();
	for (unsigned int i = 0; i < this->drawables.size(); ++i)
	{
		(*obj)->update();
		++obj;
	}*/
}
