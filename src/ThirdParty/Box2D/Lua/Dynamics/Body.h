// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_BODY_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_BODY_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

class b2Body;
struct b2BodyDef;

NS_B2_LUA_BEGIN
{
	int BodyDef(lua_State* L);
	b2BodyDef GetBodyDef(lua_State* L);

	class Body : public rainbow::lua::Bind<Body>
	{
		friend Bind;

	public:
		static void Init(lua_State*);

		explicit Body(lua_State*);

		b2Body* get() const { return body_; }
		void reset() { body_ = nullptr; }

	private:
		static int Bind(lua_State*);

		static int CreateFixture(lua_State*);
		static int DestroyFixture(lua_State*);

		static int SetTransform(lua_State*);
		static int GetPosition(lua_State*);
		static int GetAngle(lua_State*);
		static int GetWorldCenter(lua_State*);
		static int GetLocalCenter(lua_State*);

		static int SetLinearVelocity(lua_State*);
		static int GetLinearVelocity(lua_State*);
		static int SetAngularVelocity(lua_State*);
		static int GetAngularVelocity(lua_State*);

		static int ApplyForce(lua_State*);
		static int ApplyForceToCenter(lua_State*);
		static int ApplyTorque(lua_State*);
		static int ApplyLinearImpulse(lua_State*);
		static int ApplyAngularImpulse(lua_State*);

		static int GetMass(lua_State*);
		static int GetInertia(lua_State*);
		static int GetMassData(lua_State*);
		static int SetMassData(lua_State*);
		static int ResetMassData(lua_State*);

		static int GetWorldPoint(lua_State*);
		static int GetWorldVector(lua_State*);
		static int GetLocalPoint(lua_State*);
		static int GetLocalVector(lua_State*);
		static int GetLinearVelocityFromWorldPoint(lua_State*);
		static int GetLinearVelocityFromLocalPoint(lua_State*);

		static int GetLinearDamping(lua_State*);
		static int SetLinearDamping(lua_State*);
		static int GetAngularDamping(lua_State*);
		static int SetAngularDamping(lua_State*);

		static int GetGravityScale(lua_State*);
		static int SetGravityScale(lua_State*);

		static int SetBullet(lua_State*);
		static int IsBullet(lua_State*);

		static int SetSleepingAllowed(lua_State*);
		static int IsSleepingAllowed(lua_State*);
		static int SetAwake(lua_State*);
		static int IsAwake(lua_State*);
		static int SetActive(lua_State*);
		static int IsActive(lua_State*);

		static int SetFixedRotation(lua_State*);
		static int IsFixedRotation(lua_State*);

		static int GetFixtureList(lua_State*);
		static int GetJointList(lua_State*);
		static int GetContactList(lua_State*);
		static int GetNext(lua_State*);

		static int Dump(lua_State*);

		b2Body* body_;
	};
} NS_B2_LUA_END

#endif
