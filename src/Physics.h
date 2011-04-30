#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <Box2D/Box2D.h>

// Pixel to metres ratio. Box2D uses metres as the unit for measurement.
// This ratio defines how many pixels correspond to 1 Box2D "metre"
// Box2D is optimized for objects of 1x1 metre therefore it makes sense
// to define the ratio so that your most common object type is 1x1 metre.
#define PTM_RATIO 32

// Abstraction layer
typedef b2Body          Body;
typedef b2BodyDef       BodyDef;
typedef b2CircleShape   CircleShape;
typedef b2FixtureDef    Fixture;
typedef b2Joint         Joint;
typedef b2PolygonShape  PolygonShape;
typedef b2Vec2          Vec2;


/// Platform-independent physics engine.

/// This implementation extends Box2D with a few convenience methods and
/// abstractions in case of engine change. Time step has been fixed, with help
/// from UNAgames.
///
/// \see http://gafferongames.com/game-physics/fix-your-timestep/
/// \see http://www.unagames.com/blog/daniele/2010/06/fixed-time-step-implementation-box2d
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Physics
{
public:
	static Physics& Instance()
	{
		static Physics inst;
		return inst;
	}

	b2World *world;  ///< Box2D world object

	/// Create an anchor box to tie things to.
	/// \param w  Width of the box
	/// \param h  Height of the box
	/// \param x  Position of the box (x-coordinate)
	/// \param y  Position of the box (y-coordinate)
	/// \return   Body object
	b2Body *create_anchor(const float &w, const float &h, const float &x, const float &y);

	/// Create a body, given definition and fixture.
	/// \param d        Body definition
	/// \param fixture  Body fixture
	/// \param inertia  Inertia
	/// \param mass     Body mass
	/// \return         Body object
	b2Body *create_body(const b2BodyDef *d, const b2FixtureDef *fixture = 0, const float inertia = 0.0f, const float mass = 0.0f);

	/// Create a distant joint between two bodies at their anchor points.
	/// \param a  Body A
	/// \param b  Body B
	/// \return   Joint object
	b2Joint *create_joint(b2Body *a, b2Body *b);

	/// Create a distant joint between two bodies at given points.
	/// \param a      Body A
	/// \param b      Body B
	/// \param a_pos  Position of the first end of the joint
	/// \param b_pos  Position of the other end of the joint
	/// \return       Joint object
	b2Joint *create_joint(b2Body *a, b2Body *b, const b2Vec2 &a_pos, const b2Vec2 &b_pos);

	/// Define linear and/or angular damping.
	/// \param d               Body definition
	/// \param linearDamping   Linear damping
	/// \param angularDamping  Angular damping
	void define_body_damping(b2BodyDef *d, const float linearDamping = 0.0f, const float angularDamping = 0.1f);

	/// Define body position. Remember that it's better to define its position
	/// beforehand than to create the body and then move it.
	/// \param d  Body definition
	/// \param x  Position (x-coordinate)
	/// \param y  Position (y-coordinate)
	void define_body_position(b2BodyDef *d, const float x = 0.0f, const float y = 0.0f);

	/// Remove given body from the world.
	/// \param b  Body to remove
	void dispose_body(b2Body *b);

	/// Perform inter-frame interpolation.
	void interpolate();

	/// Restore the last state of all bodies.
	void restore_state();

	/// Save the current state of all bodies.
	void save_state();

	/// Advance Box2D a step forward. This implementation uses a fixed delta.
	/// \param dt  Time spent on rendering the frame
	void step(const float &dt);

protected:
	static const unsigned int
		max_steps = 5;        ///< Maximum allowed steps (prevents spiral of death)
	static const int
		p_iter = 10,          ///< Position iterations
		v_iter = 10;          ///< Velocity iterations
	static const float
		fixed_dt,             ///< Fixed delta time step
		g;                    ///< Standard gravitational acceleration value

private:
	float accumulator;        ///< Renderer time accumulator
	float accumulator_ratio;  ///< The ratio of accumulated time (after consumption) over fixed delta

	Physics();
	~Physics();

	/// Intentionally undefined.
	Physics(const Physics &);

	/// Intentionally undefined.
	Physics &operator=(const Physics &);
};


/// Structure to keep body states between iterations.

/// Necessary for the interpolation technique deployed with fixed time steps.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
struct BodyData
{
	float curr_r, prev_r;
	b2Vec2 curr_p, prev_p;
	void *data;

	BodyData(const b2Vec2 &p, const float &r) :
		curr_r(r), prev_r(r), curr_p(p), prev_p(p), data(0) { }
};

#endif
