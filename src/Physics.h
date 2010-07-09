/*
 *  Physics.h
 *  OnWire
 *
 *  Created by Tommy Nguyen on 6/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "Box2D.h"

//Pixel to metres ratio. Box2D uses metres as the unit for measurement.
//This ratio defines how many pixels correspond to 1 Box2D "metre"
//Box2D is optimized for objects of 1x1 metre therefore it makes sense
//to define the ratio so that your most common object type is 1x1 metre.
#define PTM_RATIO 32

class Physics
{
public:
	b2World *world;

	Physics();

	~Physics()
	{
		delete this->world;
	}

	void tick(float dt);
};

#endif
