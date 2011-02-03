/// The line object represents the actual line in the game.

/// Responsible for initialising the physical in-game object and updating it.
/// Implemented with vertex buffer object.
///
/// \see http://www.box2d.org/forum/viewtopic.php?f=6&t=2094
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef LINE_H_
#define LINE_H_

#include <Rainbow/OpenGL.h>
#include <Rainbow/Physics.h>

class Line
{
public:
	static const unsigned int LINE_SEGMENTS = 12;  ///< Number of segments making up the line

	Line(const float &scr_w, const float &scr_h);

	/// Applies an impulse on given segment of the line.
	inline void apply_impulse(const Vec2 &Ns, const unsigned int n) const
	{
		this->segment[n]->ApplyLinearImpulse(Ns, this->segment[n]->GetWorldCenter());
	}

	/// Uses OpenGL ES for drawing.
	void draw();

	inline const Vec2* get_displacement_at(const unsigned int n) const
	{
		return &this->segment[n]->GetPosition();//.y - this->center;
	}

	/// Updates line segment positions from the physics engine.
	void update();

protected:
	static const unsigned int
		LINE_TRIANGLES = (LINE_SEGMENTS << 1) + 4,  ///< Number of triangles needed to draw the line
		LINE_VERTICES = LINE_TRIANGLES << 1;        ///< Number of vertices needed to make the triangle strip

	const unsigned int LINE_VBO_SIZE;  ///< Vertex buffer size
	const float LINE_WIDTH;            ///< Width of the line at the bottom of the screen
	const float LINE_WIDTH_OFFSET;     ///< Width of the line at the top of the screen

private:
	const float center;               ///< Horizontal center
	GLuint buffer;                    ///< Vertex buffer object name
	GLfloat vertices[LINE_VERTICES];  ///< Triangle strip vertices
	Body *end;                        ///< Anchor at the end of the line
	Body *segment[LINE_SEGMENTS];     ///< Actual line segments
	Body *start;                      ///< Anchor at the start of the line

	friend class OnWireGame;
};

#endif
