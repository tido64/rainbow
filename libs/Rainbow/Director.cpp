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
	this->lua.load("onwire.lua");
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
