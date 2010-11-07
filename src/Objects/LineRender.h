/// A wrapper class for drawing the line with cocos2d.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#import "Line.h"

@interface LineRender : CCNode {
	GLfloat *line;
}

-(LineRender *)setData:(GLfloat *)arr;

@end
