/// The line object represents the actual line in the game.

/// Responsible for initialising the physical in-game object and updating it. On
/// other platforms than iOS, it also handles on-screen drawing.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \see http://www.box2d.org/forum/viewtopic.php?f=6&t=2094
/// \author Tommy Nguyen

#ifndef LINE_H_
#define LINE_H_

#include "../Framework/Physics.h"
#include "../Hardware/Screen.h"

static const unsigned int LINE_SEGMENTS = 12;
static const unsigned int LINE_TRIANGLES = (LINE_SEGMENTS << 1) + 4;
static const unsigned int LINE_VERTICES = LINE_TRIANGLES << 1;
static const float LINE_WIDTH = 5.0f;
static const float LINE_WIDTH_OFFSET = (LINE_WIDTH - 1.0f) / LINE_TRIANGLES;

class Line
{
public:
	/// Initialise the line with a length (usually half the screen height).
	/// \param length The length of the line
	Line(const float &length);
	~Line();

	/// Applies an impulse on given segment of the line.
	void apply_impulse(const Vec2 &Ns, const unsigned int segment);

	/// Uses OpenGL ES for drawing.
	void draw();

	/// Updates line segment positions from the physics engine.
	void update();

private:
	GLfloat *line;		///< Line segments to draw on-screen, used by OGLES
	Body
		*end,			///< Anchor at the end of the line
		**segment,		///< Actual line segments
		*start;			///< Anchor at the start of the line
	Physics *physics;	///< Physics engine pointer

	friend class OnWireGame;
};

#endif
