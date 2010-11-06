/*
 *  Line.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/30/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Line.h"

const float offset = 3.0f;

#if defined(ONWIRE_ANDROID)

Line::Line(const float l) :
	line(new GLfloat[(this->segments << 2) + 4]), physics(Physics::Instance())
{

#elif defined(ONWIRE_IOS)
#define this self

id ref_line;

@implementation Line
-(Line *) init:(const float)l
{
	self = [super init];
	self->line = new GLfloat[(LINE_SEGMENTS << 2) + 4];
	self->physics = Physics::Instance();

#endif

	const float
		d = l / PTM_RATIO / LINE_SEGMENTS,
		x = Screen::Instance()->get_width() / (PTM_RATIO << 1);

	// Create the constraints


	// Create a pendulum
	CircleShape body_shape;
	body_shape.m_radius = d;

	Fixture *body_fix = this->physics->create_fixture(&body_shape, 0.0f, 1.0f);

	BodyDef *body_def = new BodyDef();
	this->physics->define_body_damping(body_def);

	this->physics->define_body_position(body_def, x, -body_shape.m_radius + offset);
	this->start = this->physics->create_body(body_def, body_fix);
	Body *last = this->start;

	float r = d * 0.5f;
	body_shape.m_radius = r;
	delete body_fix;
	body_fix = this->physics->create_fixture(&body_shape, 0.0f, 0.1f);

	// Create rope segments and joints
	this->segment = static_cast<Body **>(calloc(LINE_SEGMENTS, sizeof(Body *)));
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

#if defined(ONWIRE_ANDROID)

	this->update();
}

Line::~Line()
{
	delete this->form;
	delete[] this->line;
	free(this->segment);
}

#elif defined(ONWIRE_IOS)

	[self update];
	ref_line = self;
	return self;
}

-(void) dealloc
{
	// don't forget to call "super dealloc"
	[super dealloc];
}

#endif

#if defined(ONWIRE_ANDROID)
void Line::apply_impulse(const Vec2 &Ns, const unsigned int n)
#elif defined(ONWIRE_IOS)
-(void) o_apply_impulse:(const Vec2 &)Ns :(const unsigned int)n
#endif
{
	this->segment[n]->ApplyLinearImpulse(Ns, this->segment[n]->GetWorldCenter());
}

#if defined(ONWIRE_ANDROID)
void Line::draw()
#elif defined(ONWIRE_IOS)
-(void) draw
#endif
{
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glLineWidth(3.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertexPointer(2, GL_FLOAT, 0, this->line);

	glDrawArrays(GL_LINES, 0, (LINE_SEGMENTS << 1) + 2);
	glLineWidth(1.0f);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
}

#if defined(ONWIRE_ANDROID)
void Line::update()
#elif defined(ONWIRE_IOS)
-(void) update
#endif
{
	unsigned int count = 0;

	Vec2 cur = this->start->GetWorldCenter();
	this->line[count] = cur.x * PTM_RATIO;
	this->line[++count] = cur.y * PTM_RATIO;

	for (unsigned int i = 0; i < LINE_SEGMENTS; ++i)
	{
		cur = this->segment[i]->GetWorldCenter();
		this->line[++count] = cur.x * PTM_RATIO;
		this->line[++count] = cur.y * PTM_RATIO;
		this->line[++count] = cur.x * PTM_RATIO;
		this->line[++count] = cur.y * PTM_RATIO;
	}

	cur = this->end->GetWorldCenter();
	this->line[++count] = cur.x * PTM_RATIO;
	this->line[++count] = cur.y * PTM_RATIO;
}

#ifdef ONWIRE_IOS

@end

void apply_impulse(const Vec2 &Ns, const unsigned int segment)
{
	[ref_line o_apply_impulse:Ns :segment];
}

#endif
