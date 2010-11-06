/// The line object represents the actual line in the game.

/// Responsible for initialising the physical in-game object and drawing it
/// on-screen.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \see http://www.box2d.org/forum/viewtopic.php?f=6&t=2094
/// \author Tommy Nguyen

#ifndef LINE_H_
#define LINE_H_

#include "../Framework/Physics.h"
#include "../Hardware/Screen.h"

static const unsigned int LINE_SEGMENTS = 12;

#if defined(ONWIRE_ANDROID)

class Line
{
public:

	/// Initialise the line with a length (usually half the screen height).
	/// \param length The length of the line
	Line(const float length);
	~Line();

	/// Applies an impulse on given segment of the line.
	void apply_impulse(const Vec2 &Ns, const unsigned int segment);

	/// Uses OpenGL ES for drawing.
	void draw();

	/// Gets updated line segment positions from the physics engine.
	void update();

private:

#elif defined(ONWIRE_IOS)

@interface Line : CCNode
{

#endif

	GLfloat *line;			///< Line segments to draw on-screen, used by OGLES
	Body
		*end,				///< Anchor at the end of the line
		**segment,			///< Actual line segments
		*start;				///< Anchor at the start of the line
	Physics *physics;		///< Points to the physics engine

#if defined(ONWIRE_ANDROID)

};

#elif defined(ONWIRE_IOS)

}

-(void) o_apply_impulse:(const Vec2 &)Ns :(const unsigned int)segment;
-(void) draw;
-(Line *) init:(const float)length;
-(void) update;

@end

void apply_impulse(const Vec2 &Ns, const unsigned int segment);

#endif

#endif
