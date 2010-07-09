/*
 *  Screen.h
 *  OnWire
 *
 *	Retrieves screen size regardless of platform.
 *
 *  Created by Tommy Nguyen on 6/26/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "Platform.h"

class Screen
{
public:
	Screen();
	inline int get_width() { return this->width; }
	inline int get_height() { return this->height; }

private:
	int width, height;
};

#endif
