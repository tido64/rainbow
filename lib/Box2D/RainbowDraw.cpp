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

#ifdef USE_PHYSICS
#include <Box2D/RainbowDraw.h>

#include <Graphics/OpenGL.h>
#include <Graphics/ShaderDetails.h>

namespace
{
	const int32 kCircleSegments = 16;
	const float32 kIncrement = 2.0f * b2_pi / static_cast<float32>(kCircleSegments);
}

RainbowDraw::RainbowDraw(const float32 ptm) :
	ptm(ptm), vertexCount(0), vertices(nullptr) { }

RainbowDraw::~RainbowDraw()
{
	delete[] this->vertices;
}

void RainbowDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	PopulateBuffer(vertices, vertexCount, color);
	SetBuffer();

	glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void RainbowDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	PopulateBuffer(vertices, vertexCount, color, 0.5f);
	SetBuffer();

	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);

	PopulateBuffer(vertices, vertexCount, color);
	glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
}

void RainbowDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	PopulateCircleBuffer(center, radius, color);
	SetBuffer();

	glDrawArrays(GL_TRIANGLE_FAN, 0, kCircleSegments);
}

void RainbowDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	PopulateCircleBuffer(center, radius, color, 0.5f);
	SetBuffer();

	glDrawArrays(GL_TRIANGLE_FAN, 0, kCircleSegments);

	PopulateCircleBuffer(center, radius, color);
	glDrawArrays(GL_LINE_LOOP, 0, kCircleSegments);

	// Draw the axis line
	DrawSegment(center, center + radius * axis, color);
}

void RainbowDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	this->vertices[0].color.r = color.r;
	this->vertices[0].color.g = color.g;
	this->vertices[0].color.b = color.b;
	this->vertices[0].color.a = 1.0f;
	this->vertices[0].vertex = p1;
	this->vertices[0].vertex *= this->ptm;
	this->vertices[1].color.r = color.r;
	this->vertices[1].color.g = color.g;
	this->vertices[1].color.b = color.b;
	this->vertices[1].color.a = 1.0f;
	this->vertices[1].vertex = p2;
	this->vertices[1].vertex *= this->ptm;
	glDrawArrays(GL_LINES, 0, 2);
}

void RainbowDraw::DrawTransform(const b2Transform& xf)
{
	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;

	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
}

void RainbowDraw::PopulateBuffer(const b2Vec2* vertices, int32 vertexCount, const b2Color& color, const float opacity)
{
	if (vertexCount > this->vertexCount)
	{
		delete[] this->vertices;
		this->vertices = new Vertex[vertexCount];
		this->vertexCount = vertexCount;
	}
	for (int32 i = 0; i < vertexCount; ++i)
	{
		this->vertices[i].color.r = color.r;
		this->vertices[i].color.g = color.g;
		this->vertices[i].color.b = color.b;
		this->vertices[i].color.a = opacity;
		this->vertices[i].vertex  = vertices[i];
		this->vertices[i].vertex *= this->ptm;
	}
}

void RainbowDraw::PopulateCircleBuffer(const b2Vec2& center, float32 radius, const b2Color& color, const float opacity)
{
	if (kCircleSegments > this->vertexCount)
	{
		delete[] this->vertices;
		this->vertices = new Vertex[kCircleSegments];
		this->vertexCount = kCircleSegments;
	}

	float32 theta = 0.0f;
	for (int32 i = 0; i < kCircleSegments; ++i)
	{
		this->vertices[i].color.r = color.r;
		this->vertices[i].color.g = color.g;
		this->vertices[i].color.b = color.b;
		this->vertices[i].color.a = opacity;
		this->vertices[i].vertex  = center + radius * b2Vec2(cosf(theta), sinf(theta));
		this->vertices[i].vertex *= this->ptm;
		theta += kIncrement;
	}
}

void RainbowDraw::SetBuffer()
{
	glVertexAttribPointer(Shader::COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), &this->vertices->color);
	glVertexAttribPointer(Shader::VERTEX, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), &this->vertices->vertex);
}

#endif
