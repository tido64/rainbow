/*
* Copyright (c) 2011 Erin Catto http://box2d.org
*
* iPhone port by Simon Oliver - http://www.handcircus.com
* Rainbow port by Bifrost Entertainment - http://bifrostent.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef RAINBOW_DRAW_H_
#define RAINBOW_DRAW_H_

#include <Box2D/Common/b2Draw.h>
#include <Common/Color.h>

class RainbowDraw : public b2Draw
{
public:
	RainbowDraw(const float32 ptm);
	virtual ~RainbowDraw();

	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	virtual void DrawTransform(const b2Transform& xf);

private:
	float32 ptm;
	int32 vertexCount;
	struct Vertex
	{
		Colorf color;
		b2Vec2 vertex;
	} *vertices;

	void PopulateBuffer(const b2Vec2* vertices, int32 vertexCount, const b2Color& color, const float opacity = 1.0f);
	void PopulateCircleBuffer(const b2Vec2& center, float32 radius, const b2Color& color, const float opacity = 1.0f);
	void SetBuffer();
};

#endif
