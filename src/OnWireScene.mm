//
//  OnWireScene.mm
//  OnWire
//
//  Created by Tommy Nguyen on 6/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "OnWireScene.h"

@implementation GameUI

-(id) init
{
	if((self=[super init]))
	{
		//self.isTouchEnabled = YES; // enable touches

		// Initialise game master
		master = Master::Instance();
		master->reset(9000);
		master->add(&hud);

		// Add HUD elements
		[self addChild:hud.get_distance()->get_label()];
		[self addChild:hud.get_distance_label()->get_label()];
		[self addChild:hud.get_time()->get_label()];
		[self addChild:hud.get_time_label()->get_label()];

		// Set up timer
		[self schedule: @selector(tick) interval:1.0f];
	}
	return self;
}

-(void) tick
{
	master->elapse_time();
}

-(void) dealloc
{
	[super dealloc];
}

@end

@implementation OnWire

+(id) scene
{
	// 'scene' is an autorelease object.
	CCScene *scene = [CCScene node];

	// 'layer' is an autorelease object.
	OnWire *layer = [OnWire node];
	GameUI *ui = [GameUI node];

	// add layer as a child to scene
	[scene addChild: layer];
	[scene addChild: ui];

	// return the scene
	return scene;
}

-(id) init
{
	if((self=[super init]))
	{
		//self.isTouchEnabled = YES; // enable touches
		self.isAccelerometerEnabled = YES; // enable accelerometer

		// Physics
		physics = Physics::Instance();

		// Physics debugging
		debug_draw = new GLESDebugDraw(PTM_RATIO);
		physics->world->SetDebugDraw(debug_draw);

		uint32 flags = 0;
		flags += b2DebugDraw::e_shapeBit;
		flags += b2DebugDraw::e_jointBit;
		//flags += b2DebugDraw::e_aabbBit;
		//flags += b2DebugDraw::e_pairBit;
		//flags += b2DebugDraw::e_centerOfMassBit;
		debug_draw->SetFlags(flags);

		// Create and add sprites
		skyline = new Skyline();
		building = new Building();
		line = [[Line alloc] init: Screen::Instance()->get_height() >> 1];
		//[self addChild:skyline->get_object()];
		//[self addChild:building->get_object()];
		[self addChild:line];
		Master::Instance()->add(line);

		[self schedule: @selector(tick:)]; // interval:timestep];

		// Controls
		//[[CCTouchDispatcher sharedDispatcher] addTargetedDelegate:this priority:0 swallowsTouches:YES];
	}
	return self;
}

-(void) draw
{
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states:  GL_VERTEX_ARRAY,
	// Unneeded states: GL_TEXTURE_2D, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	physics->world->DrawDebugData();

	// restore default GL states
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

-(void) tick: (ccTime) dt
{
	//building->update();
	physics->step(dt);
	[line update];

	//Iterate over the bodies in the physics world
	for (b2Body *b = physics->world->GetBodyList(); b; b = b->GetNext())
	{
		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		if (d != 0 && d->sprite != 0)
		{
			//Synchronize the AtlasSprites position and rotation with the corresponding body
			CCSprite *myActor = (CCSprite*)b->GetUserData();
			myActor.position = CGPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO);
			myActor.rotation = -1 * CC_RADIANS_TO_DEGREES(b->GetAngle());
		}
	}
}

// on "dealloc" you need to release all your retained objects
-(void) dealloc
{
	delete building;
	delete skyline;
	delete debug_draw;

	// don't forget to call "super dealloc"
	[super dealloc];
}

@end
