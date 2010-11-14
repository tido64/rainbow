/// The line object represents the actual line in the game.

/// Responsible for initialising the physical in-game object and updating it. On
/// other platforms than iOS, it also handles on-screen drawing.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \see http://www.box2d.org/forum/viewtopic.php?f=6&t=2094
/// \author Tommy Nguyen

#ifndef LINE_H_
#define LINE_H_

#include <OpenGLES/ES1/gl.h>
#include "../Framework/Physics.h"

const unsigned int
	LINE_SEGMENTS = 12,											///< Number of segments making up the line
	LINE_TRIANGLES = (LINE_SEGMENTS << 1) + 4,					///< Number of triangles needed to draw the line
	LINE_VERTICES = LINE_TRIANGLES << 1;						///< Number of vertices needed to make a triangle strip
const float
	LINE_WIDTH = 5.0f,											///< Width of the line at the bottom of the screen
	LINE_WIDTH_OFFSET = (LINE_WIDTH - 1.0f) / LINE_TRIANGLES;	///< Width of the line at the top of the screen

class Line
{
public:
	Line(const float &scr_w, const float &scr_h);
	~Line();

	/// Applies an impulse on given segment of the line.
	inline void apply_impulse(const Vec2 &Ns, const unsigned int n)
	{
		this->segment[n]->ApplyLinearImpulse(Ns, this->segment[n]->GetWorldCenter());
	}

	/// Uses OpenGL ES for drawing.
	void draw();

	inline float get_displacement_at(const unsigned int n)
	{
		return this->segment[n]->GetPosition().y - this->center;
	}

	/// Updates line segment positions from the physics engine.
	void update();

private:
	const float center;	///< Horizontal center
	GLfloat *line;		///< Line segments to draw on-screen, used by OGLES
	Body
		*end,			///< Anchor at the end of the line
		**segment,		///< Actual line segments
		*start;			///< Anchor at the start of the line
	Physics *physics;	///< Physics engine pointer

	friend class OnWireGame;
};

#endif
