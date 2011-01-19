//
//  OnWireScene.h
//  OnWire
//
//  Created by Tommy Nguyen on 6/26/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#import "GLES-Render.h"
#import "OnWireGame.h"

@interface OnWire : CCLayer {
	Controls *controls;
	GLESDebugDraw *debug_draw;
	OnWireGame *game;
	Rainbow::TouchArray get_touches;
}

+(id) scene;

@end
