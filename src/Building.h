/*
 *  Building.h
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BUILDING_H_
#define BUILDING_H_

#include "Master.h"
#include "Screen.h"
#include "Sprite.h"

class Building : public Sprite
{
public:
	Building();
	~Building() { }
	void update();

private:
	float initial, progress;
};

#endif
