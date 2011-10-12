#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <Box2D/Box2D.h>

#include "Algorithm.h"
#include "Platform.h"

// Pixel to metres ratio. Box2D uses metres as the unit for measurement.
// This ratio defines how many pixels correspond to 1 Box2D "metre"
// Box2D is optimized for objects of 1x1 metre therefore it makes sense
// to define the ratio so that your most common object type is 1x1 metre.
#define PTM_RATIO 32
#define MTP_RATIO (1.0f / PTM_RATIO)

// Abstraction layer
typedef b2Body          Body;
typedef b2BodyDef       BodyDef;
typedef b2CircleShape   CircleShape;
typedef b2FixtureDef    Fixture;
typedef b2Joint         Joint;
typedef b2PolygonShape  PolygonShape;
typedef b2Vec2          Vec2;

/// Platform-independent physics engine.
///
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
	/// Structure to keep body states between iterations.
	///
	/// Necessary for the interpolation technique deployed with fixed time steps.
	///
	/// Copyright 2010-11 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	struct BodyData
	{
		float curr_r, prev_r;
		void *data;
		b2Vec2 curr_p, prev_p;

		BodyData(const b2BodyDef &d) :
			curr_r(d.angle), prev_r(d.angle), data(0), curr_p(d.position), prev_p(d.position) { }
	};

	static Physics& Instance();

	/// Apply a force at a world point. If the force is not applied at the
	/// center of mass, it will generate a torque and affect the angular
	/// velocity.
	/// \param b   The body to apply force on
	/// \param vx  Force vector, usually in Newtons (N)
	/// \param vy  Force vector, usually in Newtons (N)
	/// \param px  The world position of the point of application
	/// \param py  The world position of the point of application
	void apply_force(void *b, const float vx, const float vy, const float px, const float py);

	/// Apply an angular impulse.
	/// \param b        The body to apply angular impulse on
	/// \param impulse  The angular impulse in units of kg*m^2/s
	void apply_angular_impulse(void *b, float impulse);

	/// Apply a torque. This affects the angular velocity without affecting the
	/// linear velocity of the center of mass.
	/// \param torque  About the z-axis (out of the screen), usually in N-m.
	void apply_torque(void *b, float torque);

	/// Create an immovable box. Useful as platforms.
	/// \param x  Position of the box (x-coordinate)
	/// \param y  Position of the box (y-coordinate)
	/// \param w  Width of the box
	/// \param h  Height of the box
	/// \return   Body object
	b2Body* create_anchor(const float x, const float y, const float w, const float h);

	b2Body* create_circle(const float x, const float y, const float radius, const float friction = 0.2f, const float restitution = 0.0f, const float density = 0.0f);

	/// Create a distant joint between two bodies at their anchor points.
	/// \param a  Body A
	/// \param b  Body B
	/// \return   Joint object
	b2Joint* create_distance_joint(void *a, void *b);

	/// Create a distant joint between two bodies at specified points.
	/// \param a      Body A
	/// \param b      Body B
	/// \param a_pos  Position of the first end of the joint
	/// \param b_pos  Position of the other end of the joint
	/// \return       Joint object
	b2Joint* create_distance_joint(b2Body *a, b2Body *b, const b2Vec2 &a_pos, const b2Vec2 &b_pos);

	b2Body* create_rope(float x0, float y0, float x1, float y1, unsigned int segments = 0);

	/// Create the world. Deletes the old one.
	/// \param g_x    Gravity in x-direction, defaults to 0.0f
	/// \param g_y    Gravity in y-direction, defaults to -9.80665f
	/// \param sleep  Enable sleeping objects, defaults to true
	void create_world(const float g_x = 0.0f, const float g_y = kStandardGravity);

	/// Destroy all bodies.
	void destroy();

	/// Remove specified body from the world.
	/// \param b  Body to remove
	void destroy(void *b);

	/// Set linear and/or angular damping.
	/// \param d               Body definition
	/// \param linearDamping   Linear damping
	/// \param angularDamping  Angular damping
	void set_body_damping(void *b, const float linearDamping = 0.0f, const float angularDamping = 0.1f);

	/// Change the gravitational forces.
	/// \param g_x  Gravity in x-direction, defaults to 0.0f
	/// \param g_y  Gravity in y-direction, defaults to -9.80665f
	void set_gravity(const float g_x = 0.0f, const float g_y = kStandardGravity);

	/// Advance Box2D a step forward. This implementation uses a fixed delta.
	/// \param dt  Time spent rendering the frame
	void step(const float &dt);

protected:
	static const unsigned int
		max_steps = 5;        ///< Maximum allowed steps (prevents spiral of death)
	static const int
		p_iter = 10,          ///< Position iterations
		v_iter = 10;          ///< Velocity iterations
	static const float
		fixed_dt,             ///< Fixed delta time step
		inv_fixed_dt;         ///< Inversed fixed delta

private:
	float accumulator;        ///< Renderer time accumulator
	float accumulator_ratio;  ///< The ratio of accumulated time (after consumption) over fixed delta

	b2World *world;           ///< Box2D world object

	Physics();
	~Physics();

	/// Intentionally left undefined.
	Physics(const Physics &);

	/// Interpolate between current and next frame.
	void interpolate();

	/// Restore the last state of all bodies.
	void restore_state();

	/// Save the current state of all bodies.
	void save_state();

	/// Intentionally left undefined.
	Physics &operator=(const Physics &);
};

inline Physics& Physics::Instance()
{
	static Physics inst;
	return inst;
}

inline void Physics::apply_force(void *b, const float vx, const float vy, const float px, const float py)
{
	static_cast<b2Body *>(b)->ApplyForce(b2Vec2(vx, vy), b2Vec2(px, py));
}

inline void Physics::apply_angular_impulse(void *b, float impulse)
{
	static_cast<b2Body *>(b)->ApplyAngularImpulse(impulse);
}

inline void Physics::apply_torque(void *b, float torque)
{
	static_cast<b2Body *>(b)->ApplyTorque(torque);
}

inline b2Joint *Physics::create_distance_joint(void *a, void *b)
{
	return this->create_distance_joint(static_cast<b2Body *>(a), static_cast<b2Body *>(b), static_cast<b2Body *>(a)->GetWorldCenter(), static_cast<b2Body *>(b)->GetWorldCenter());
}

inline void Physics::set_gravity(const float g_x, const float g_y)
{
	this->world->SetGravity(b2Vec2(g_x, g_y));
}

#endif
