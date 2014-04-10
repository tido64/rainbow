// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <Box2D/RainbowDraw.h>

#include "Common/Constants.h"

NS_B2_LUA_BEGIN
{
	namespace
	{
		int g_contact = LUA_REFNIL;  ///< Cached contact object for all contact events.

		void CreateContactList(lua_State *L, b2Contact *contact)
		{
			if (g_contact == LUA_REFNIL)
			{
				Rainbow::Lua::alloc<Contact>(L);
				g_contact = luaL_ref(L, LUA_REGISTRYINDEX);
			}
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_contact);
			lua_rawgeti(L, -1, 0);
			Contact *c = Rainbow::Lua::touserdata<Contact>(L, -1);
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

		template<typename F, typename... Args>
		void ForEachDynamicBody(b2World &world, F f, Args&&... args)
		{
			for (b2Body *b = world.GetBodyList(); b; b = b->GetNext())
			{
				if (b->GetType() == b2_staticBody)
					continue;

				f(b, std::forward<Args>(args)...);
			}
		}

		void PreContactEvent(const char *const event, lua_State *L, int listener)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, listener);
			lua_getfield(L, -1, event);
			lua_insert(L, -2);
		}
	}

	class World : public b2ContactListener, public Bind<World>
	{
		friend Bind;

	public:
		World(lua_State *);
		virtual ~World();

		/* Implement b2ContactListener. */

		virtual void BeginContact(b2Contact *contact) override;
		virtual void EndContact(b2Contact *contact) override;
		virtual void PreSolve(b2Contact *contact,
		                      const b2Manifold *oldManifold) override;
		virtual void PostSolve(b2Contact *contact,
		                       const b2ContactImpulse *impulse) override;

	private:
		static int set_contact_listener(lua_State *);

		static int create_body(lua_State *);
		static int destroy_body(lua_State *);

		static int create_joint(lua_State *);
		static int destroy_joint(lua_State *);

		static int step(lua_State *);

		static int clear_forces(lua_State *);

		static int set_gravity(lua_State *);
		static int get_gravity(lua_State *);

		static int dump(lua_State *);

		int contact_listener;
		lua_Number elapsed;
		lua_State *L;
		RainbowDraw debug_draw;
		b2World world;

		void interpolate();
		void restore_state();
		void save_state();
	};

	const unsigned int kMaxSteps = 5;
	const lua_Number kFixedStep = 1.0 / 60.0;
	const lua_Number kFramesPerMs = 60.0 / 1000.0;

	World::World(lua_State *L)
	    : contact_listener(LUA_REFNIL), elapsed(0.0), L(L),
	      debug_draw(ptm_ratio), world(b2Vec2(0.0f, kStandardGravity))
	{
		LUA_ASSERT(lua_isnone(L, 1) ||
		           (lua_isnumber(L, 1) && lua_isnumber(L, 2)),
		           "b2.World(gx = 0.0, gy = -9.80665)");

		if (lua_gettop(L) >= 1)
		{
			const b2Vec2 &gravity = this->world.GetGravity();
			this->world.SetGravity(b2Vec2(luaR_optnumber(L, 1, gravity.x),
			                              luaR_optnumber(L, 2, gravity.y)));
		}

		this->debug_draw.SetFlags(b2Draw::e_shapeBit);
		this->world.SetDebugDraw(&this->debug_draw);

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
		LUA_ASSERT(lua_istable(L, 2),
		           "<b2.World>:SetContactListener(listener)");

		World *self = Bind::self(L);
		if (!self)
			return 0;

		if (self->contact_listener == LUA_REFNIL)
			self->world.SetContactListener(self);
		else
			luaL_unref(L, LUA_REGISTRYINDEX, self->contact_listener);
		self->contact_listener = luaL_ref(L, LUA_REGISTRYINDEX);
		return 0;
	}

	int World::create_body(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 2), "<b2.World>:CreateBody(<b2.BodyDef>)");

		World *self = Bind::self(L);
		if (!self)
			return 0;

		b2BodyDef def;
		parse_BodyDef(L, def);
		def.userData = new BodyData(def);
		b2Body *body = self->world.CreateBody(&def);
		lua_pushlightuserdata(L, body);
		Rainbow::Lua::alloc<Body>(L);

		// Register body
		lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
		lua_pushlightuserdata(L, body);
		lua_pushvalue(L, -3);
		lua_rawset(L, -3);
		lua_pop(L, 1);

		return 1;
	}

	int World::destroy_body(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "<b2.World>:DestroyBody(<b2.Body>)");

		World *self = Bind::self(L);
		if (!self)
			return 0;

		b2Body *body = Rainbow::Lua::touserdata<Body>(L, 2)->get();

		// Unregister body
		lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
		lua_pushlightuserdata(L, body);
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1);

		delete static_cast<BodyData*>(body->GetUserData());
		self->world.DestroyBody(body);
		return 0;
	}

	int World::step(lua_State *L)
	{
		LUA_ASSERT(
		    lua_isnumber(L, 2),
		    "<b2.World>:Step(dt[, velocityIterations, positionIterations])");

		World *self = Bind::self(L);
		if (!self)
			return 0;

		self->elapsed += lua_tonumber(L, 2);
		unsigned int steps =
		    static_cast<unsigned int>(self->elapsed * kFramesPerMs);
		if (!steps)
			self->restore_state();
		else
		{
			self->elapsed -= steps / kFramesPerMs;
			if (steps > kMaxSteps)
				steps = kMaxSteps;

			const int v_iter = luaR_optinteger(L, 2, 8);
			const int p_iter = luaR_optinteger(L, 3, 3);

			self->restore_state();
			for (unsigned int i = 0; i < steps; ++i)
			{
				self->world.Step(kFixedStep, v_iter, p_iter);
				self->save_state();
			}
			self->world.ClearForces();
			self->interpolate();
		}
		return 0;
	}

	int World::clear_forces(lua_State *L)
	{
		World *self = Bind::self(L);
		if (!self)
			return 0;

		self->world.ClearForces();
		return 0;
	}

	int World::set_gravity(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) && lua_isnumber(L, 3),
		           "<b2.World>:SetGravity(x, y)");

		World *self = Bind::self(L);
		if (!self)
			return 0;

		self->world.SetGravity(b2Vec2(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}

	int World::get_gravity(lua_State *L)
	{
		World *self = Bind::self(L);
		if (!self)
			return 0;

		const b2Vec2 &gravity = self->world.GetGravity();
		lua_pushnumber(L, gravity.x);
		lua_pushnumber(L, gravity.y);
		return 2;
	}

	int World::dump(lua_State *L)
	{
		World *self = Bind::self(L);
		if (!self)
			return 0;

		self->world.Dump();
		return 0;
	}

	void World::BeginContact(b2Contact *contact)
	{
		if (this->contact_listener == LUA_REFNIL)
			return;

		const char event[] = "BeginContact";
		PreContactEvent(event, this->L, this->contact_listener);
		CreateContactList(this->L, contact);
		FireContactEvent(event, this->L, 1);
	}

	void World::EndContact(b2Contact *contact)
	{
		if (this->contact_listener == LUA_REFNIL)
			return;

		const char event[] = "EndContact";
		PreContactEvent(event, this->L, this->contact_listener);
		CreateContactList(this->L, contact);
		FireContactEvent(event, this->L, 1);
	}

	void World::PreSolve(b2Contact *contact, const b2Manifold *)
	{
		if (this->contact_listener == LUA_REFNIL)
			return;

		const char event[] = "PreSolve";
		PreContactEvent(event, this->L, this->contact_listener);
		CreateContactList(this->L, contact);
		FireContactEvent(event, this->L, 1);
	}

	void World::PostSolve(b2Contact *contact, const b2ContactImpulse *)
	{
		if (this->contact_listener == LUA_REFNIL)
			return;

		const char event[] = "PostSolve";
		PreContactEvent(event, this->L, this->contact_listener);
		CreateContactList(this->L, contact);
		FireContactEvent(event, this->L, 1);
	}

	void World::interpolate()
	{
		const float ratio = this->elapsed * kFramesPerMs;
		ForEachDynamicBody(this->world, [](b2Body *b, const float ratio, const float rest) {
			if (!b->IsAwake())
				return;

			BodyData *d = static_cast<BodyData*>(b->GetUserData());
			if (!d->sprite)
				return;

			const b2Vec2 v = ptm_ratio * (ratio * d->curr_p + rest * d->prev_p);
			d->sprite->set_position(Vec2f(v.x, v.y));
			d->sprite->set_rotation(ratio * d->curr_r + rest * d->prev_r);
		}, ratio, 1.0f - ratio);
	}

	void World::restore_state()
	{
		ForEachDynamicBody(this->world, [](b2Body *b) {
			BodyData *d = static_cast<BodyData*>(b->GetUserData());
			if (!d->sprite)
				return;

			const Vec2f position(d->curr_p.x * ptm_ratio, d->curr_p.y * ptm_ratio);
			d->sprite->set_position(position);
			d->sprite->set_rotation(d->curr_r);
		});
	}

	void World::save_state()
	{
		ForEachDynamicBody(this->world, [](b2Body *b) {
			BodyData *d = static_cast<BodyData*>(b->GetUserData());
			b2Transform t = b->GetTransform();
			d->prev_p = d->curr_p;
			d->curr_p = t.p;
			d->prev_r = d->curr_r;
			d->curr_r = t.q.GetAngle();
		});
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::Lua::World;

	template<>
	const char World::Bind::class_name[] = "World";

	template<>
	const bool World::Bind::is_constructible = true;

	template<>
	const luaL_Reg World::Bind::functions[] = {
		{ "SetContactListener",  &World::set_contact_listener },
		{ "CreateBody",          &World::create_body },
		{ "DestroyBody",         &World::destroy_body },
		{ "Step",                &World::step },
		{ "SetGravity",          &World::set_gravity },
		{ "GetGravity",          &World::get_gravity },
		{ "Dump",                &World::dump },
		{ nullptr,               nullptr }
	};
} NS_RAINBOW_LUA_END
