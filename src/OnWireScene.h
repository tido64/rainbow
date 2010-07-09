//
//  OnWireScene.h
//  OnWire
//
//  Created by Tommy Nguyen on 6/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "GLES-Render.h"

#include "Assets.h"
#include "Physics.h"
#include "Screen.h"

@interface OnWire : CCLayer {
	GLESDebugDraw *debug_draw;
	Physics physics;
	Screen screen;

	// Sprites
	CCSprite *building;
	Sprite *skyline;
}

+(id) scene; // returns a Scene that contains the HelloWorld as the only child

@end
