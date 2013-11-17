// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <Box2D/RainbowDraw.h>

#include "Common/Constants.h"

namespace b2
{
	namespace Lua
	{
		namespace
		{
			int g_contact = LUA_REFNIL;  ///< Cached contact object for all contact events.

			void CreateContactList(lua_State *L, b2Contact* contact)
			{
				if (g_contact == LUA_REFNIL)
				{
					Rainbow::Lua::alloc<Contact>(L);
					g_contact = luaL_ref(L, LUA_REGISTRYINDEX);
				}
				lua_rawgeti(L, LUA_REGISTRYINDEX, g_contact);
				lua_rawgeti(L, -1, 0);
				Contact *c = static_cast<Contact*>(luaR_touserdata(L, -1, Contact::class_name));
				c->set(contact);
				lua_pop(L, 1);
			}

			void FireContactEvent(const char *const event, lua_State *L, int nargs)
			{
				const int lua_e = lua_pcall(L, nargs + 1, 0, 0);
				if (lua_e != LUA_OK)
				{
					Rainbow::Lua::error(L, lua_e);
					luaL_error(L, "Failed to evaluate '%s'", event);
				}
			}

			void PreContactEvent(const char *const event, lua_State *L, int listener)
			{
				lua_rawgeti(L, LUA_REGISTRYINDEX, listener);
				lua_getfield(L, -1, event);
				lua_insert(L, -2);
			}
		}

		class World :
			public b2ContactListener,
			public b2World,
			public Rainbow::Lua::Bind<World, b2World, Rainbow::Lua::kBindTypeDerived>
		{
			friend class Rainbow::Lua::Bind<World, b2World, Rainbow::Lua::kBindTypeDerived>;

		public:
			World(lua_State *);
			virtual ~World();

			/* Implement b2ContactListener. */

			virtual void BeginContact(b2Contact* contact);
			virtual void EndContact(b2Contact* contact);
			virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
			virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

		private:
			int contact_listener;
			lua_Number elapsed;

			lua_State *L;

			RainbowDraw debug_draw;

			int set_contact_listener(lua_State *);

			int create_body(lua_State *);
			int destroy_body(lua_State *);

			int create_joint(lua_State *);
			int destroy_joint(lua_State *);

			int step(lua_State *);

			int clear_forces(lua_State *);

			int set_gravity(lua_State *);
			int get_gravity(lua_State *);

			int dump(lua_State *);

			void interpolate();
			void restore_state();
			void save_state();
		};

		const unsigned int kMaxSteps = 5;
		const lua_Number kFixedStep = 1.0 / 60.0;
		const lua_Number kFramesPerMs = 60.0 / 1000.0;

		World::World(lua_State *L) :
			b2World(b2Vec2(0.0f, kStandardGravity)), contact_listener(LUA_REFNIL),
			elapsed(0.0), L(L), debug_draw(ptm_ratio)
		{
			if (lua_gettop(L) >= 2)
				b2World::SetGravity(b2Vec2(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));

			b2World::SetDebugDraw(&this->debug_draw);
			this->debug_draw.SetFlags(b2Draw::e_shapeBit);

			g_debug_data->push_back(this);
		}

		World::~World()
		{
			g_debug_data->erase(this);
			if (this->contact_listener != LUA_REFNIL)
				luaL_unref(L, LUA_REGISTRYINDEX, this->contact_listener);
		}

		int World::set_contact_listener(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.World>:SetContactListener(listener)");

			if (this->contact_listener == LUA_REFNIL)
				b2World::SetContactListener(this);
			else
				luaL_unref(L, LUA_REGISTRYINDEX, this->contact_listener);
			this->contact_listener = luaL_ref(L, LUA_REGISTRYINDEX);
			return 0;
		}

		int World::create_body(lua_State *L)
		{
			LUA_ASSERT(lua_type(L, 1) == LUA_TTABLE,
			           "<b2.World>:CreateBody(b2.BodyDef)");

			b2BodyDef def;
			parse_BodyDef(L, def);
			def.userData = new BodyData(def);
			b2Body *body = b2World::CreateBody(&def);
			lua_pushlightuserdata(L, body);
			Rainbow::Lua::alloc<Body>(L);

			// Register body
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(body));
			lua_pushvalue(L, -3);
			lua_rawset(L, -3);
			lua_pop(L, 1);

			return 1;
		}

		int World::destroy_body(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.World>:DestroyBody(b2.Body)");

			b2Body *body = Rainbow::Lua::wrapper<Body>(L)->get();

			// Unregister body
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(body));
			lua_pushnil(L);
			lua_rawset(L, -3);
			lua_pop(L, 1);

			delete static_cast<BodyData*>(body->GetUserData());
			b2World::DestroyBody(body);
			return 0;
		}

		int World::step(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 3,
			           "<b2.World>:Step(dt[, velocityIterations, positionIterations])");

			this->elapsed += luaR_tonumber(L, 1);
			unsigned int steps = static_cast<unsigned int>(this->elapsed * kFramesPerMs);
			if (!steps)
				this->restore_state();
			else
			{
				this->elapsed -= steps / kFramesPerMs;
				if (steps > kMaxSteps)
					steps = kMaxSteps;

				const int v_iter = luaR_optinteger(L, 2, 8);
				const int p_iter = luaR_optinteger(L, 3, 3);

				this->restore_state();
				for (unsigned int i = 0; i < steps; ++i)
				{
					b2World::Step(kFixedStep, v_iter, p_iter);
					this->save_state();
				}
				b2World::ClearForces();
				this->interpolate();
			}
			return 0;
		}

		int World::clear_forces(lua_State *)
		{
			b2World::ClearForces();
			return 0;
		}

		int World::set_gravity(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<b2.World>:SetGravity(x, y)");

			b2World::SetGravity(b2Vec2(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));
			return 0;
		}

		int World::get_gravity(lua_State *L)
		{
			b2Vec2 gravity = b2World::GetGravity();
			lua_pushnumber(L, gravity.x);
			lua_pushnumber(L, gravity.y);
			return 2;
		}

		int World::dump(lua_State *)
		{
			b2World::Dump();
			return 0;
		}

		void World::BeginContact(b2Contact* contact)
		{
			if (this->contact_listener == LUA_REFNIL)
				return;

			const char event[] = "BeginContact";
			PreContactEvent(event, this->L, this->contact_listener);
			CreateContactList(this->L, contact);
			FireContactEvent(event, this->L, 1);
		}

		void World::EndContact(b2Contact* contact)
		{
			if (this->contact_listener == LUA_REFNIL)
				return;

			const char event[] = "EndContact";
			PreContactEvent(event, this->L, this->contact_listener);
			CreateContactList(this->L, contact);
			FireContactEvent(event, this->L, 1);
		}

		void World::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
		{
			if (this->contact_listener == LUA_REFNIL)
				return;

			const char event[] = "PreSolve";
			PreContactEvent(event, this->L, this->contact_listener);
			CreateContactList(this->L, contact);
			static_cast<void>(oldManifold);
			FireContactEvent(event, this->L, 1);
		}

		void World::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
		{
			if (this->contact_listener == LUA_REFNIL)
				return;

			const char event[] = "PostSolve";
			PreContactEvent(event, this->L, this->contact_listener);
			CreateContactList(this->L, contact);
			static_cast<void>(impulse);
			FireContactEvent(event, this->L, 1);
		}

		void World::interpolate()
		{
			const float ratio = this->elapsed * kFramesPerMs;
			const float rest = 1.0f - ratio;
			for (b2Body *b = b2World::GetBodyList(); b; b = b->GetNext())
			{
				if (b->GetType() == b2_staticBody || !b->IsAwake())
					continue;

				BodyData *d = static_cast<BodyData*>(b->GetUserData());
				if (!d->sprite)
					continue;

				const b2Vec2 v = ptm_ratio * (ratio * d->curr_p + rest * d->prev_p);
				d->sprite->set_position(Vec2f(v.x, v.y));
				d->sprite->set_rotation(ratio * d->curr_r + rest * d->prev_r);
			}
		}

		void World::restore_state()
		{
			for (b2Body *b = b2World::GetBodyList(); b; b = b->GetNext())
			{
				if (b->GetType() == b2_staticBody)
					continue;

				BodyData *d = static_cast<BodyData*>(b->GetUserData());
				if (!d->sprite)
					continue;

				const Vec2f position(d->curr_p.x * ptm_ratio, d->curr_p.y * ptm_ratio);
				d->sprite->set_position(position);
				d->sprite->set_rotation(d->curr_r);
			}
		}

		void World::save_state()
		{
			for (b2Body *b = b2World::GetBodyList(); b; b = b->GetNext())
			{
				if (b->GetType() == b2_staticBody)
					continue;

				BodyData *d = static_cast<BodyData*>(b->GetUserData());
				b2Transform t = b->GetTransform();
				d->prev_p = d->curr_p;
				d->curr_p = t.p;
				d->prev_r = d->curr_r;
				d->curr_r = t.q.GetAngle();
			}
		}
	}
}

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<b2::Lua::World, b2World, kBindTypeDerived> b2LuaWorld;

		template<>
		const char b2LuaWorld::class_name[] = "World";

		template<>
		const Method<b2::Lua::World> b2LuaWorld::methods[] = {
			{ "SetContactListener",  &b2::Lua::World::set_contact_listener },
			{ "CreateBody",          &b2::Lua::World::create_body },
			{ "DestroyBody",         &b2::Lua::World::destroy_body },
			{ "Step",                &b2::Lua::World::step },
			{ "SetGravity",          &b2::Lua::World::set_gravity },
			{ "GetGravity",          &b2::Lua::World::get_gravity },
			{ "Dump",                &b2::Lua::World::dump },
			{ nullptr,               nullptr }
		};
	}
}
