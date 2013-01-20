#include <Box2D/RainbowDraw.h>

#include "Common/Constants.h"

namespace b2
{
	namespace Lua
	{
		namespace
		{
			int g_contact = -1;  ///< Cached contact object for all contact events.

			void CreateContactList(lua_State *L, b2Contact* contact)
			{
				if (g_contact < 0)
				{
					Rainbow::Lua::alloc<Contact>(L);
					g_contact = luaL_ref(L, LUA_REGISTRYINDEX);
				}
				lua_rawgeti(L, LUA_REGISTRYINDEX, g_contact);
				lua_rawgeti(L, -1, 0);
				Contact **ptr = static_cast<Contact**>(lua_touserdata(L, -1));
				(*ptr)->contact = contact;
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

		class World : public b2ContactListener
		{
		public:
			static const char class_name[];
			static const Rainbow::Lua::Method<World> methods[];

			World(lua_State *);
			virtual ~World();

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

			/* Implement b2ContactListener. */

			virtual void BeginContact(b2Contact* contact);
			virtual void EndContact(b2Contact* contact);
			virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
			virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

		private:
			float elapsed;
			int contact_listener;

			lua_State *L;

			RainbowDraw debug_draw;
			b2World world;

			void interpolate();
			void restore_state();
			void save_state();
		};

		const float fixed_step = 1.0f / 60.0f;
		const float fpms = 0.06f;
		const unsigned int max_steps = 5;

		const char World::class_name[] = "World";
		const Rainbow::Lua::Method<World> World::methods[] = {
			{ "SetContactListener",  &World::set_contact_listener },
			{ "CreateBody",          &World::create_body },
			{ "DestroyBody",         &World::destroy_body },
			{ "Step",                &World::step },
			{ "SetGravity",          &World::set_gravity },
			{ "GetGravity",          &World::get_gravity },
			{ "Dump",                &World::dump },
			{ nullptr,               nullptr }
		};

		World::World(lua_State *L) :
			elapsed(0.0f), contact_listener(LUA_REFNIL), L(L),
			debug_draw(ptm_ratio), world(b2Vec2(0.0f, kStandardGravity))
		{
			if (lua_gettop(L) >= 2)
				this->world.SetGravity(b2Vec2(lua_tonumber(L, 1), lua_tonumber(L, 2)));

			this->world.SetDebugDraw(&this->debug_draw);
			this->debug_draw.SetFlags(b2Draw::e_shapeBit);

			g_debug_data->push_back(&this->world);
		}

		World::~World()
		{
			g_debug_data->erase(&this->world);
			if (this->contact_listener != LUA_REFNIL)
				luaL_unref(L, LUA_REGISTRYINDEX, this->contact_listener);
		}

		int World::set_contact_listener(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.World>:SetContactListener(listener)");

			if (this->contact_listener == LUA_REFNIL)
				this->world.SetContactListener(this);
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
			b2Body *body = this->world.CreateBody(&def);
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

			b2Body *body = Rainbow::Lua::wrapper<Body>(L)->body;

			// Unregister body
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(body));
			lua_pushnil(L);
			lua_rawset(L, -3);
			lua_pop(L, 1);

			delete static_cast<BodyData*>(body->GetUserData());
			this->world.DestroyBody(body);
			return 0;
		}

		int World::step(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 3,
			           "<b2.World>:Step(dt[, velocityIterations, positionIterations])");

			this->elapsed += lua_tonumber(L, 1);
			unsigned int steps = static_cast<unsigned int>(this->elapsed * fpms);
			if (!steps)
				this->restore_state();
			else
			{
				this->elapsed -= steps / fpms;
				if (steps > max_steps)
					steps = max_steps;

				int v_iter = 8, p_iter = 3;
				if (lua_gettop(L) == 3)
				{
					v_iter = lua_tointeger(L, 2);
					p_iter = lua_tointeger(L, 3);
				}

				this->restore_state();
				for (unsigned int i = 0; i < steps; ++i)
				{
					this->world.Step(fixed_step, v_iter, p_iter);
					this->save_state();
				}
				this->world.ClearForces();
				this->interpolate();
			}
			return 0;
		}

		int World::clear_forces(lua_State *)
		{
			this->world.ClearForces();
			return 0;
		}

		int World::set_gravity(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<b2.World>:SetGravity(x, y)");

			this->world.SetGravity(b2Vec2(lua_tonumber(L, 1), lua_tonumber(L, 2)));
			return 0;
		}

		int World::get_gravity(lua_State *L)
		{
			b2Vec2 gravity = this->world.GetGravity();
			lua_pushnumber(L, gravity.x);
			lua_pushnumber(L, gravity.y);
			return 2;
		}

		int World::dump(lua_State *)
		{
			this->world.Dump();
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
			const float ratio = this->elapsed * fpms;
			const float rest = 1.0f - ratio;
			for (b2Body *b = this->world.GetBodyList(); b; b = b->GetNext())
			{
				if (b->GetType() == b2_staticBody || !b->IsAwake())
					continue;

				BodyData *d = static_cast<BodyData*>(b->GetUserData());
				const b2Vec2 v = ratio * d->curr_p + rest * d->prev_p;
				const float r = ratio * d->curr_r + rest * d->prev_r;
				if (!d->sprite)
					b->SetTransform(v, r);
				else
				{
					d->sprite->set_position(v.x * ptm_ratio, v.y * ptm_ratio);
					d->sprite->set_rotation(r);
				}
			}
		}

		void World::restore_state()
		{
			for (b2Body *b = this->world.GetBodyList(); b; b = b->GetNext())
			{
				if (b->GetType() == b2_staticBody)
					continue;

				BodyData *d = static_cast<BodyData*>(b->GetUserData());
				if (!d->sprite)
					b->SetTransform(d->curr_p, d->curr_r);
				else
				{
					d->sprite->set_position(d->curr_p.x * ptm_ratio, d->curr_p.y * ptm_ratio);
					d->sprite->set_rotation(d->curr_r);
				}
			}
		}

		void World::save_state()
		{
			for (b2Body *b = this->world.GetBodyList(); b; b = b->GetNext())
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
