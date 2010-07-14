/*
 *  Screen.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 6/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Screen.h"

Screen::Screen()
{
#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

	[[CCDirector sharedDirector] setDeviceOrientation: CCDeviceOrientationPortrait];
	CGSize screenSize = [CCDirector sharedDirector].winSize;

	this->width = screenSize.width;
	this->height = screenSize.height;

#endif
}

Screen *Screen::Instance()
{
	static Screen inst;
	return &inst;
}