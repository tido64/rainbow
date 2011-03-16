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

	Screen::Instance().init();
	const char *script = AssetManager::Instance().get_full_path("onwire.lua");
	this->lua.load(script);

#else

	this->lua.load("onwire.lua");

#endif

	this->lua.call("init");
	this->lua.call("update");
}

void Director::draw()
{
	this->lua.call("draw");
}

void Director::update()
{
	this->lua.call("update");
	// Update sprites and such here
}
