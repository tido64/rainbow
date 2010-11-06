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

#include "../Framework/Sprite.h"
#include "../Hardware/Screen.h"
#include "Master.h"

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
