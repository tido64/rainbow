/*
 *  Director.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 2/13/11.
 *  Copyright 2011 Bifrost Games. All rights reserved.
 *
 */

#include "Director.h"

Director::Director()
{
#ifdef RAINBOW_IOS

	// Asset source set in Lua constructor
	this->update_video();

#else

	AssetManager::Instance().set_source("assets.bfz");

#endif
}

void Director::draw()
{
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
	Physics::Instance().step(dt);
	this->lua.input.update(this->lua.L);
	this->lua.call("update");

	// Update sprites and such here
}
