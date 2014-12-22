// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_WORLD_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_WORLD_H_

#include <memory>

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include "Lua/LuaBind.h"

namespace b2
{
	class StableWorld;

	namespace Lua
	{
		class World : public b2ContactListener, public Rainbow::Lua::Bind<World>
		{
			friend Bind;

		public:
			explicit World(lua_State *);
			~World();

			StableWorld* get() const { return world_.get(); }

			// Implement b2ContactListener

			void BeginContact(b2Contact *) override;
			void EndContact(b2Contact *) override;
			void PreSolve(b2Contact *, const b2Manifold *) override;
			void PostSolve(b2Contact *, const b2ContactImpulse *) override;

		private:
			static int GetPTM(lua_State *);
			static int SetPTM(lua_State *);

			static int SetDestructionListener(lua_State *);
			static int SetContactFilter(lua_State *);
			static int SetContactListener(lua_State *);
			static int SetDebugDraw(lua_State *);
			static int CreateBody(lua_State *);
			static int DestroyBody(lua_State *);
			static int CreateJoint(lua_State *);
			static int DestroyJoint(lua_State *);
			static int Step(lua_State *);
			static int QueryAABB(lua_State *);
			static int RayCast(lua_State *);
			static int GetBodyList(lua_State *);
			static int GetJointList(lua_State *);
			static int GetContactList(lua_State *);
			static int SetAllowSleeping(lua_State *);
			static int GetAllowSleeping(lua_State *);
			static int SetWarmStarting(lua_State *);
			static int GetWarmStarting(lua_State *);
			static int SetContinuousPhysics(lua_State *);
			static int GetContinuousPhysics(lua_State *);
			static int SetSubStepping(lua_State *);
			static int GetSubStepping(lua_State *);
			static int GetProxyCount(lua_State *);
			static int GetBodyCount(lua_State *);
			static int GetJointCount(lua_State *);
			static int GetContactCount(lua_State *);
			static int GetTreeHeight(lua_State *);
			static int GetTreeBalance(lua_State *);
			static int GetTreeQuality(lua_State *);
			static int SetGravity(lua_State *);
			static int GetGravity(lua_State *);
			static int IsLocked(lua_State *);
			static int ShiftOrigin(lua_State *);
			static int GetContactManager(lua_State *);
			static int GetProfile(lua_State *);
			static int Dump(lua_State *);

			std::unique_ptr<StableWorld> world_;
			int contact_listener_;
			lua_State *state_;
		};
	}
}

#endif
