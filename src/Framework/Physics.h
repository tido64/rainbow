/// Platform-independent physics engine.

/// This implementation extends Box2D with a few convenience methods and
/// abstractions in case of engine change. Time step has been fixed, with help
/// from UNAgames.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \see http://gafferongames.com/game-physics/fix-your-timestep/
/// \see http://www.unagames.com/blog/daniele/2010/06/fixed-time-step-implementation-box2d
/// \author Tommy Nguyen

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "../Hardware/Platform.h"
#include "Box2D.h"

// Pixel to metres ratio. Box2D uses metres as the unit for measurement.
// This ratio defines how many pixels correspond to 1 Box2D "metre"
// Box2D is optimized for objects of 1x1 metre therefore it makes sense
// to define the ratio so that your most common object type is 1x1 metre.
#define PTM_RATIO 32

// Abstraction layer
typedef b2Body			Body;
typedef b2BodyDef		BodyDef;
typedef b2CircleShape	CircleShape;
typedef b2FixtureDef	Fixture;
typedef b2PolygonShape	PolygonShape;
typedef b2Vec2			Vec2;

class Physics
{
public:
	b2World *world;	///< Box2D world object

	/// Physics is a singleton.
	static Physics *Instance();
	~Physics();

	/// Creates an anchor bpx to tie things to.
	/// \param w Width of the box
	/// \param h Height of the box
	/// \param x Position of the box (x-coordinate)
	/// \param y Position of the box (y-coordinate)
	/// \return  Body object
	b2Body *create_anchor(const float w, const float h, const float x, const float y);

	/// Creates a body, given definition and fixture.
	/// \param d Body definition
	/// \param f Body fixture
	/// \return  Body object
	b2Body *create_body(const b2BodyDef *d, const b2FixtureDef *fixture = 0, const float inertia = 0.0f, const float mass = 0.0f);

	/// Creates a distant joint between two bodies.
	/// \param a		Body A
	/// \param b		Body B
	/// \param a_pos	Position of the first end of the joint
	/// \param b_pos	Position of the other end of the joint
	/// \return			Joint object
	b2Joint *create_joint(b2Body *a, b2Body *b, const b2Vec2 &a_pos, const b2Vec2 &b_pos);

	/// Defines linear and/or angular damping.
	/// \param d				Body definition
	/// \param linearDamping	Linear damping
	/// \param angularDamping	Angular damping
	void define_body_damping(b2BodyDef *d, const float linearDamping = 0.0f, const float angularDamping = 0.1f);

	/// Defines body position. Remember that it's better to define its position
	/// beforehand than to create the body and then move it.
	/// \param d Body definition
	/// \param x Position (x-coordinate)
	/// \param y Position (y-coordinate)
	void define_body_position(b2BodyDef *d, const float x = 0.0f, const float y = 0.0f);

	void dispose_body(b2Body *);

	/// Performs inter-frame interpolation.
	void interpolate();

	/// Restores the last state of all bodies.
	void restore_state();

	/// Saves the current state of all bodies.
	void save_state();

	/// Advances Box2D a step forward.
	/// \param dt Time spent on rendering the frame.
	void step(float dt);

private:
	static const unsigned int
		max_steps = 5;					///< Maximum allowed steps (prevents spiral of death)
	static const int
		p_iter = 1,						///< Position iterations
		v_iter = 8;						///< Velocity iterations
	static const float
		fixed_timestep = 1.f / 60.f,	///< Fixed time step
		g = -9.80665f;					///< Standard gravitational acceleration value
	float
		accumulator,					///< Renderer time accumulator
		accumulator_ratio;				///< The ratio of accumulator (after consumption) over fixed timestep

	/// Initialises the physics engine. In this implementation, the Box2D world
	/// and its properties.
	Physics();
};

/// Structure to keep body states between iterations. Necessary for the
/// interpolation technique with fixed time steps.
struct BodyData
{
	float curr_r, prev_r;
	b2Vec2 curr_p, prev_p;
	CCSprite *sprite;

	BodyData(const b2Vec2 &p, const float &r) : sprite(0)
	{
		prev_r = r;
		curr_r = r;
		prev_p = p;
		curr_p = p;
	}
};

#endif
