//
//  OnWireScene.mm
//  OnWire
//
//  Created by Tommy Nguyen on 6/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "OnWireScene.h"

@implementation OnWire

+(id) scene
{
	// 'scene' is an autorelease object.
	CCScene *scene = [CCScene node];
	
	// 'layer' is an autorelease object.
	OnWire *layer = [OnWire node];
	
	// add layer as a child to scene
	[scene addChild: layer];
	
	// return the scene
	return scene;
}

-(id) init
{
	if( (self=[super init]))
	{
		self.isTouchEnabled = YES; // enable touches
		self.isAccelerometerEnabled = YES; // enable accelerometer

		// Physics
		debug_draw = new GLESDebugDraw(PTM_RATIO);
		physics.world->SetDebugDraw(debug_draw);

		unsigned int flags = 0;
		flags += b2DebugDraw::e_shapeBit;
		//flags += b2DebugDraw::e_jointBit;
		//flags += b2DebugDraw::e_aabbBit;
		//flags += b2DebugDraw::e_pairBit;
		//flags += b2DebugDraw::e_centerOfMassBit;
		debug_draw->SetFlags(flags);

		// Add sprites
		skyline = new Skyline(screen);
		[self addChild:skyline->get_object()];

		//building = [CCSprite spriteWithFile: @"Icon.png"];
		//building.position = ccp(screen.get_width() / 2, screen.get_height() / 2);
		//[self addChild: building];

		[self schedule: @selector(tick:)];

		// Controls
		//[[CCTouchDispatcher sharedDispatcher] addTargetedDelegate:this priority:0 swallowsTouches:YES];

/* Physics? Ain't ready for that yet...

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
		
		//Set up sprite
		
		CCSpriteSheet *sheet = [CCSpriteSheet spriteSheetWithFile:@"blocks.png" capacity:150];
		[self addChild:sheet z:0 tag:kTagSpriteSheet];
		
		[self addNewSpriteWithCoords:ccp(screen->get_width() / 2, screen->get_height() / 2)];
		
		CCLabel *label = [CCLabel labelWithString:@"Tap screen" fontName:@"Marker Felt" fontSize:32];
		[self addChild:label z:0];
		[label setColor:ccc3(0,0,255)];
		label.position = ccp( screenSize.width/2, screenSize.height-50);
		
		[self schedule: @selector(tick:)];
 */
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

	physics.world->DrawDebugData();

	// restore default GL states
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

-(void) tick: (ccTime) dt
{
	physics.tick(dt);

	//Iterate over the bodies in the physics world
	for (b2Body *b = physics.world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL)
		{
			//Synchronize the AtlasSprites position and rotation with the corresponding body
			CCSprite *myActor = (CCSprite*)b->GetUserData();
			myActor.position = CGPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO);
			myActor.rotation = -1 * CC_RADIANS_TO_DEGREES(b->GetAngle());
		}	
	}
}

// on "dealloc" you need to release all your retained objects
- (void) dealloc
{
	delete skyline;
	delete building;
	delete debug_draw;

	// don't forget to call "super dealloc"
	[super dealloc];
}

@end
