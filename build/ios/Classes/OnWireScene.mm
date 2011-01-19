//
//  OnWireScene.mm
//  OnWire
//
//  Created by Tommy Nguyen on 6/26/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#import "OnWireScene.h"

@implementation OnWire

+(id)scene
{
	CCScene *scene = [CCScene node];  // 'scene' is an autorelease object
	OnWire *layer = [OnWire node];    // 'layer' is an autorelease object
	[scene addChild:layer];           // add 'layer' as a child to scene
	return scene;                     // return the scene
}

-(id)init
{
	if((self=[super init]))
	{
		self.isTouchEnabled = YES;
		self.isAccelerometerEnabled = YES;

		// Initialise screen
		Screen::Instance().init();

		// Physics
		debug_draw = new GLESDebugDraw(PTM_RATIO);
		Physics::Instance().world->SetDebugDraw(debug_draw);

		uint32 flags = 0;
		flags += b2DebugDraw::e_shapeBit;
		flags += b2DebugDraw::e_jointBit;
		//flags += b2DebugDraw::e_aabbBit;
		//flags += b2DebugDraw::e_pairBit;
		//flags += b2DebugDraw::e_centerOfMassBit;
		debug_draw->SetFlags(flags);

		// Initialise game
		AssetManager::Instance()->set_source();
		controls = Controls::Instance();
		game = new OnWireGame();
		game->reset(9000);

		[self schedule: @selector(clock) interval:1.0f];
		[self schedule: @selector(tick:)];
	}
	return self;
}

-(void)clock
{
	game->tick();
}

-(void)draw
{
	// Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Needed states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY
	// Unneeded states: GL_COLOR_ARRAY
	glDisableClientState(GL_COLOR_ARRAY);

	game->draw();

	// Needed states: GL_VERTEX_ARRAY,
	// Unneeded states: GL_TEXTURE_2D, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
	//glDisable(GL_TEXTURE_2D);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//physics->world->DrawDebugData();

	// restore default GL states
	//glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

-(void)tick:(ccTime)dt
{
	Physics::Instance().step(dt);
	game->update();
}

-(void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	const unsigned int c = touches.count;
	Touch *t = get_touches(touches);
	controls->touch_began(t, c);
	delete[] t;
}

-(void)ccTouchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	const unsigned int c = touches.count;
	Touch *t = get_touches(touches);
	controls->touch_moved(t, c);
	delete[] t;
}

-(void)ccTouchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	const unsigned int c = touches.count;
	Touch *t = get_touches(touches);
	controls->touch_ended(t, c);
	delete[] t;
}

-(void)ccTouchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	controls->touch_canceled();
}

// on "dealloc" you need to release all your retained objects
-(void)dealloc
{
	delete controls;
	delete game;
	delete debug_draw;

	// don't forget to call "super dealloc"
	[super dealloc];
}

@end
