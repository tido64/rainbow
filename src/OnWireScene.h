//
//  OnWireScene.h
//  OnWire
//
//  Created by Tommy Nguyen on 6/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "GLES-Render.h"
#import "Objects/LineRender.h"

#include "OnWireGame.h"
#include "Physics.h"

@interface GameUI : CCLayer {
	OnWireGame *game;
}

@end

@interface OnWire : CCLayer {
	GLESDebugDraw *debug_draw;
	LineRender *line;
	OnWireGame *game;
	Physics *physics;
}

+(id) scene;

@end
