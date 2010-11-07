//
//  LineRender.mm
//  OnWire
//
//  Created by Tommy Nguyen on 11/7/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "LineRender.h"

@implementation LineRender

-(LineRender *)setData:(GLfloat *)arr
{
	line = arr;
	return self;
}

-(void)draw
{
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertexPointer(2, GL_FLOAT, 0, line);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, LINE_TRIANGLES);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
}

@end
