//
//  Physics.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include <Rainbow/Physics.h>

Physics::Physics() : accumulator(0.0f), accumulator_ratio(0.0f)
{
	// Define gravity vector
	b2Vec2 gravity(0.0f, Physics::g);

	// Define the world
	this->world = new b2World(gravity, true);
	this->world->SetAutoClearForces(false);
	this->world->SetContinuousPhysics(true);
}

Physics::~Physics()
{
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
		this->dispose_body(b);
	delete this->world;
}

b2Body *Physics::create_anchor(const float &w, const float &h, const float &x, const float &y)
{
	b2BodyDef body_def;
	body_def.position.x = x;
	body_def.position.y = y;
	b2Body *b = this->world->CreateBody(&body_def);
	b2PolygonShape box;
	box.SetAsBox(w, h);
	b->CreateFixture(&box, 0.0f);
	return b;
}

b2Body *Physics::create_body(const b2BodyDef *d, const b2FixtureDef *fixture, const float inertia, const float mass)
{
	b2Body *b = this->world->CreateBody(d);
	//b2MassData data;
	//data.I = inertia;
	//data.mass = mass;
	//b->SetMassData(&data);
	if (fixture != 0) b->CreateFixture(fixture);
	BodyData *u = new BodyData(d->position, d->angle);
	b->SetUserData(u);
	return b;
}

b2Joint *Physics::create_joint(b2Body *a, b2Body *b)
{
	b2DistanceJointDef joint_def;
	joint_def.Initialize(a, b, a->GetPosition(), b->GetPosition());
	return this->world->CreateJoint(&joint_def);
}

b2Joint *Physics::create_joint(b2Body *a, b2Body *b, const b2Vec2 &a_pos, const b2Vec2 &b_pos)
{
	b2DistanceJointDef joint_def;
	joint_def.Initialize(a, b, a_pos, b_pos);
	return this->world->CreateJoint(&joint_def);
}

void Physics::define_body_damping(b2BodyDef *d, const float linearDamping, const float angularDamping)
{
	d->type = b2_dynamicBody;
	d->linearDamping = linearDamping;
	d->angularDamping = angularDamping;
}

void Physics::define_body_position(b2BodyDef *d, const float x, const float y)
{
	d->position.x = x;
	d->position.y = y;
}

void Physics::dispose_body(b2Body *b)
{
	if (b->GetUserData() != 0)
	{
		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		delete d;
	}
	this->world->DestroyBody(b);
}

void Physics::interpolate()
{
	this->accumulator_ratio = this->accumulator / Physics::fixed_dt;
	const float rest = 1.0f - this->accumulator_ratio;
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody || !b->IsAwake()) continue;

		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		b->SetTransform(
			this->accumulator_ratio * d->curr_p + rest * d->prev_p,
			this->accumulator_ratio * d->curr_r + rest * d->prev_r);
	}
}

void Physics::restore_state()
{
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody) continue;

		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		b->SetTransform(d->curr_p, d->curr_r);
	}
}

void Physics::save_state()
{
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody) continue;

		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		b2Transform t = b->GetTransform();
		d->prev_p = d->curr_p;
		d->curr_p = t.position;
		d->prev_r = d->curr_r;
		d->curr_r = t.GetAngle();
	}
}

void Physics::step(const float &dt)
{
	this->accumulator += dt;
	unsigned int n_steps = static_cast<unsigned int>(this->accumulator / Physics::fixed_dt);
	if (n_steps == 0)
	{
		this->restore_state();
		return;
	}

	this->accumulator -= n_steps * Physics::fixed_dt;
	if (n_steps > Physics::max_steps) n_steps = Physics::max_steps;

	this->restore_state();
	for (unsigned int i = 0; i < n_steps; ++i)
	{
		this->world->Step(Physics::fixed_dt, Physics::v_iter, Physics::p_iter);
		this->save_state();
	}
	this->world->ClearForces();
	this->interpolate();
}
