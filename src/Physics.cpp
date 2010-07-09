/*
 *  Physics.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 6/27/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Physics.h"

Physics::Physics()
{
	// Define gravity vector
	b2Vec2 gravity;
	gravity.Set(0.0f, -10.0f);

	// Define the world
	this->world = new b2World(gravity, true);
	this->world->SetContinuousPhysics(true);

/* Physics? Ain't ready for that yet...

	uint32 flags = 0;
	flags += b2DebugDraw::e_shapeBit;
	//		flags += b2DebugDraw::e_jointBit;
	//		flags += b2DebugDraw::e_aabbBit;
	//		flags += b2DebugDraw::e_pairBit;
	//		flags += b2DebugDraw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);

	// Define the ground body.
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0); // bottom-left corner

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	b2Body* groundBody = world->CreateBody(&groundBodyDef);

	// Define the ground box shape.
	b2PolygonShape groundBox;

	// bottom
	groundBox.SetAsEdge(b2Vec2(0, 0), b2Vec2(screen->get_width()/PTM_RATIO, 0));
	groundBody->CreateFixture(&groundBox, 0);

	// top
	groundBox.SetAsEdge(b2Vec2(0, screen->get_height() / PTM_RATIO), b2Vec2(screen->get_width() / PTM_RATIO, screen->get_height() / PTM_RATIO));
	groundBody->CreateFixture(&groundBox, 0);

	// left
	groundBox.SetAsEdge(b2Vec2(0, screen->get_height() / PTM_RATIO), b2Vec2(0, 0));
	groundBody->CreateFixture(&groundBox, 0);

	// right
	groundBox.SetAsEdge(b2Vec2(screen->get_width() / PTM_RATIO, screen->get_height() / PTM_RATIO), b2Vec2(screen->get_width() / PTM_RATIO, 0));
	groundBody->CreateFixture(&groundBox, 0);
*/
}

void Physics::tick(float dt)
{
	//It is recommended that a fixed time step is used with Box2D for stability
	//of the simulation, however, we are using a variable time step here.
	//You need to make an informed choice, the following URL is useful
	//http://gafferongames.com/game-physics/fix-your-timestep/

	int velocityIterations = 8;
	int positionIterations = 1;

	this->world->Step(dt, velocityIterations, positionIterations);
}
