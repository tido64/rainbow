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

@interface GameUI : CCLayer {
	HUD hud;
	Master *master;
}

@end

@interface OnWire : CCLayer {
	GLESDebugDraw *debug_draw;
	Physics physics;

	// Sprites
	Sprite *building;
	Sprite *skyline;
}

+(id) scene;

@end
