//
//  Physics.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Physics.h"

const float Physics::fixed_dt = 1.0f / 60.0f;
const float Physics::inv_fixed_dt = 60.0f;

Physics::Physics() : accumulator(0.0f), accumulator_ratio(0.0f), world(0) { }

Physics::~Physics()
{
	this->destroy();
	delete this->world;
}

b2Body* Physics::create_anchor(const float x, const float y, const float w, const float h)
{
	b2BodyDef def;
	def.position.Set(x, y);
	def.type = b2_staticBody;
	b2Body *b = this->world->CreateBody(&def);
	b2PolygonShape box;
	box.SetAsBox(w * 0.5f, h * 0.5f);
	b->CreateFixture(&box, 0.0f);
	return b;
}

b2Body* Physics::create_circle(const float x, const float y, const float r, const float friction, const float /*restitution*/, const float density)
{
	b2CircleShape shape;
	shape.m_radius = r * MTP_RATIO;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.friction = friction;
	fixture.density = density;

	b2BodyDef body_def;
	body_def.userData = new BodyData(body_def);
	body_def.position.Set(x * MTP_RATIO, y * MTP_RATIO);
	body_def.type = b2_dynamicBody;

	b2Body *body = this->world->CreateBody(&body_def);
	body->CreateFixture(&fixture);
	return body;
}

b2Joint* Physics::create_distance_joint(b2Body *a, b2Body *b, const b2Vec2 &a_pos, const b2Vec2 &b_pos)
{
	b2DistanceJointDef joint_def;
	joint_def.Initialize(a, b, a_pos, b_pos);
	return this->world->CreateJoint(&joint_def);
}

b2Body* Physics::create_rope(float x0, float y0, float x1, float y1, unsigned int segments)
{
	x0 *= MTP_RATIO;
	y0 *= MTP_RATIO;
	x1 *= MTP_RATIO;
	y1 *= MTP_RATIO;

	if (!segments)
		segments = static_cast<unsigned int>(Rainbow::distance(x0, y0, x1, y1) * 0.025f);

	const b2Vec2 delta ((x1 - x0) / segments, (y1 - y0) / segments);

	b2CircleShape shape;
	shape.m_radius = delta.Length() * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.1f;
	fixture.friction = 0.0f;

	b2BodyDef def;
	def.userData = new BodyData(def);
	def.position.Set(x0, y0);
	def.type = b2_dynamicBody;

	b2DistanceJointDef joint;

	b2Body *pbody = this->world->CreateBody(&def);
	pbody->CreateFixture(&fixture);
	for (unsigned int i = 1; i < segments; ++i)
	{
		def.position += delta;
		def.userData = new BodyData(def);
		b2Body *body = this->world->CreateBody(&def);
		body->CreateFixture(&fixture);
		joint.Initialize(pbody, body, pbody->GetPosition(), body->GetPosition());
		this->world->CreateJoint(&joint);
		pbody = body;
	}

	return this->world->GetBodyList();
}

void Physics::create_world(const float g_x, const float g_y, bool sleep)
{
	if (!this->world)
	{
		this->world = new b2World(b2Vec2(g_x, g_y), sleep);
		this->world->SetAutoClearForces(false);
		this->world->SetContinuousPhysics(true);
	}
	else
	{
		this->destroy();
		this->world->SetGravity(b2Vec2(g_x, g_y));
	}
}

void Physics::destroy()
{
	if (!this->world)
		return;

	for (b2Joint *j = this->world->GetJointList(); j; j = j->GetNext())
		this->world->DestroyJoint(j);
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
		this->destroy(b);
}

void Physics::destroy(void *b)
{
	delete static_cast<BodyData *>(static_cast<b2Body *>(b)->GetUserData());
	this->world->DestroyBody(static_cast<b2Body *>(b));
}

void Physics::set_body_damping(void *b, const float linearDamping, const float angularDamping)
{
	static_cast<b2Body *>(b)->SetLinearDamping(linearDamping);
	static_cast<b2Body *>(b)->SetAngularDamping(angularDamping);
}

void Physics::step(const float &dt)
{
	if (!this->world)
		return;

	this->accumulator += dt;
	unsigned int n_steps = static_cast<unsigned int>(this->accumulator * Physics::inv_fixed_dt);
	if (n_steps == 0)
	{
		this->restore_state();
		return;
	}

	this->accumulator -= n_steps * Physics::fixed_dt;
	if (n_steps > Physics::max_steps)
		n_steps = Physics::max_steps;

	this->restore_state();
	for (unsigned int i = 0; i < n_steps; ++i)
	{
		this->world->Step(Physics::fixed_dt, Physics::v_iter, Physics::p_iter);
		this->save_state();
	}
	this->world->ClearForces();
	this->interpolate();
}

void Physics::interpolate()
{
	this->accumulator_ratio = this->accumulator * Physics::inv_fixed_dt;
	const float rest = 1.0f - this->accumulator_ratio;
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody || !b->IsAwake())
			continue;

		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		b->SetTransform(this->accumulator_ratio * d->curr_p + rest * d->prev_p,
						this->accumulator_ratio * d->curr_r + rest * d->prev_r);
	}
}

void Physics::restore_state()
{
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody)
			continue;

		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		b->SetTransform(d->curr_p, d->curr_r);
	}
}

void Physics::save_state()
{
	for (b2Body *b = this->world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetType() == b2_staticBody)
			continue;

		BodyData *d = static_cast<BodyData *>(b->GetUserData());
		b2Transform t = b->GetTransform();
		d->prev_p = d->curr_p;
		d->curr_p = t.position;
		d->prev_r = d->curr_r;
		d->curr_r = t.GetAngle();
	}
}
