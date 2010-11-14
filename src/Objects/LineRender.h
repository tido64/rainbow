/// A wrapper class for drawing the line within cocos2d's main loop.

/// This class is iOS specific.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#import "cocos2d.h"
#import "Line.h"

@interface LineRender : CCNode {
	const GLfloat *line;
}

-(LineRender *)setData:(const GLfloat *)arr;

@end
