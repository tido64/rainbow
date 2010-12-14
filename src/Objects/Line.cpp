//
//  Line.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 7/30/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "Line.h"

const float offset = 0.0f;  ///< For debugging purposes only.

Line::Line(const float &scr_w, const float &scr_h) :
	center(scr_w * 0.5f), physics(Physics::Instance())
{
	const float
		l = scr_h * 0.5f,
		d = l / PTM_RATIO / LINE_SEGMENTS,
		x = scr_w / (PTM_RATIO << 1);

	// Create a pendulum
	CircleShape body_shape;
	body_shape.m_radius = d;

	Fixture *body_fix = new Fixture();
	body_fix->shape = &body_shape;
	body_fix->friction = 0.0f;
	body_fix->density = 1.0f;

	BodyDef *body_def = new BodyDef();
	this->physics->define_body_damping(body_def, 0.1f, 0.2f);

	this->physics->define_body_position(body_def, x, -body_shape.m_radius + offset);
	this->start = this->physics->create_body(body_def, body_fix);

	float r = d * 0.5f;
	body_shape.m_radius = r;
	body_fix->density = 0.1f;

	// Create rope segments and joints
	this->physics->define_body_position(body_def, x, r + offset);
	Body *b = this->physics->create_body(body_def, body_fix);
	this->physics->create_joint(this->start, b);
	this->segment[0] = b;
	for (unsigned int i = 1; i < LINE_SEGMENTS; ++i)
	{
		r += d;
		this->physics->define_body_position(body_def, x, r + offset);

		b = this->physics->create_body(body_def, body_fix);
		this->physics->create_joint(this->segment[i - 1], b);
		this->segment[i] = b;
	}
	delete body_def;
	delete body_fix;

	// Attach the other end of the line to the building
	this->end = this->physics->create_anchor(0.1f, 0.1f, x, l / PTM_RATIO + offset);
	this->physics->create_joint(this->segment[LINE_SEGMENTS - 1], this->end);

	// Create constraints
	body_def = new BodyDef();
	this->physics->define_body_position(body_def, 0.0f, 0.0f);
	Body *c = this->physics->create_body(body_def);
	delete body_def;
	PolygonShape c_shape;

	r = 0.0f + offset;
	float p = x - d - 2 / PTM_RATIO;
	c_shape.SetAsEdge(Vec2(p, r), Vec2(p + 0.1f, r));
	c->CreateFixture(&c_shape, 0.0f);

	p = x + d + 2 / PTM_RATIO;
	c_shape.SetAsEdge(Vec2(p - 0.1f, r), Vec2(p, r));
	c->CreateFixture(&c_shape, 0.0f);

	glGenBuffers(1, &this->buffer);
	this->update();
}

void Line::draw()
{
	glDisable(GL_TEXTURE_2D);

	glColor4ub(0x00, 0x00, 0x00, 0xff);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, LINE_TRIANGLES);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_TEXTURE_2D);
}

void Line::update()
{
	unsigned int count = 0;
	float offset = LINE_WIDTH * 0.5f;

	Vec2 cur = this->start->GetWorldCenter();
	float x = cur.x * PTM_RATIO;
	float y = cur.y * PTM_RATIO;
	this->vertices[count] = x - offset;
	this->vertices[++count] = y;
	this->vertices[++count] = x + offset;
	this->vertices[++count] = y;

	for (unsigned int i = 0; i < LINE_SEGMENTS; ++i)
	{
		offset -= LINE_WIDTH_OFFSET;
		cur = this->segment[i]->GetWorldCenter();
		x = cur.x * PTM_RATIO;
		y = cur.y * PTM_RATIO;
		this->vertices[++count] = x - offset;
		this->vertices[++count] = y;
		this->vertices[++count] = x + offset;
		this->vertices[++count] = y;
	}

	cur = this->end->GetWorldCenter();
	x = cur.x * PTM_RATIO;
	y = cur.y * PTM_RATIO;
	this->vertices[++count] = x - offset;
	this->vertices[++count] = y;
	this->vertices[++count] = x + offset;
	this->vertices[++count] = y;

	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, VBO_SIZE, this->vertices, GL_DYNAMIC_DRAW);
}
