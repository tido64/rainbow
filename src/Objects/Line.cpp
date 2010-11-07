/*
 *  Line.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/30/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Line.h"

const float diameter	= 5.0f;
const float offset		= 0.0f;

Line::Line(const float &l) :
	line(new GLfloat[LINE_VERTICES]), physics(Physics::Instance())
{
	const float
		d = l / PTM_RATIO / LINE_SEGMENTS,
		x = Screen::Instance()->get_width() / (PTM_RATIO << 1);

	// Create a pendulum
	CircleShape body_shape;
	body_shape.m_radius = d;

	Fixture *body_fix = new Fixture();
	body_fix->shape = &body_shape;
	body_fix->friction = 0.0f;
	body_fix->density = 1.0f;

	BodyDef *body_def = new BodyDef();
	this->physics->define_body_damping(body_def, 0.3f);

	this->physics->define_body_position(body_def, x, -body_shape.m_radius + offset);
	this->start = this->physics->create_body(body_def, body_fix);
	Body *last = this->start;

	float r = d * 0.5f;
	body_shape.m_radius = r;
	body_fix->density = 0.1f;

	// Create rope segments and joints
	this->segment = new Body*[LINE_SEGMENTS];
	for (unsigned int i = 0; i < LINE_SEGMENTS; ++i)
	{
		this->physics->define_body_position(body_def, x, r + offset);
		r += d;

		Body *b = this->physics->create_body(body_def, body_fix);
		this->physics->create_joint(last, b, last->GetPosition(), b->GetPosition());

		this->segment[i] = b;
		last = b;
	}

	// Attach the other end of the line to the building
	this->end = this->physics->create_anchor(0.1f, 0.1f, x, l / PTM_RATIO + offset);
	this->physics->create_joint(last, this->end, last->GetPosition(), this->end->GetPosition());

	delete body_def;
	delete body_fix;

	// Create constraints
	body_def = new BodyDef();
	this->physics->define_body_position(body_def, 0.f, 0.f);

	Body *c = this->physics->create_body(body_def);
	PolygonShape c_shape;

	r = d * 0.5f;
	float p = x - d - 2 / PTM_RATIO;
	c_shape.SetAsEdge(Vec2(p, r - d + offset), Vec2(p, offset -d - r));
	c->CreateFixture(&c_shape, 0.f);

	p = x + d + 2 / PTM_RATIO;
	c_shape.SetAsEdge(Vec2(p, r - d + offset), Vec2(p, offset -d - r));
	c->CreateFixture(&c_shape, 0.f);

	this->update();
}

Line::~Line()
{
	delete[] this->line;
	delete[] this->segment;
}

void Line::apply_impulse(const Vec2 &Ns, const unsigned int n)
{
	this->segment[n]->ApplyLinearImpulse(Ns, this->segment[n]->GetWorldCenter());
}

void Line::draw()
{
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertexPointer(2, GL_FLOAT, 0, this->line);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, LINE_TRIANGLES);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
}

void Line::update()
{
	unsigned int count = 0;
	float offset = LINE_WIDTH * 0.5f;

	Vec2 cur = this->start->GetWorldCenter();
	float x = cur.x * PTM_RATIO;
	float y = cur.y * PTM_RATIO;
	this->line[count] = x - offset;
	this->line[++count] = y;
	this->line[++count] = x + offset;
	this->line[++count] = y;

	for (unsigned int i = 0; i < LINE_SEGMENTS; ++i)
	{
		offset -= LINE_WIDTH_OFFSET;
		cur = this->segment[i]->GetWorldCenter();
		x = cur.x * PTM_RATIO;
		y = cur.y * PTM_RATIO;
		this->line[++count] = x - offset;
		this->line[++count] = y;
		this->line[++count] = x + offset;
		this->line[++count] = y;
	}

	cur = this->end->GetWorldCenter();
	x = cur.x * PTM_RATIO;
	y = cur.y * PTM_RATIO;
	this->line[++count] = x - offset;
	this->line[++count] = y;
	this->line[++count] = x + offset;
	this->line[++count] = y;
}
