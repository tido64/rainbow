// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_B2BODY_H_
#define THIRDPARTY_BOX2D_LUA_B2BODY_H_

#include "Graphics/Sprite.h"
#include "Lua/lua_Sprite.h"

NS_B2_LUA_BEGIN
{
	struct BodyData
	{
		Sprite::Ref sprite;
		b2Vec2 curr_p, prev_p;
		float curr_r, prev_r;

		BodyData(const b2BodyDef &d)
		    : curr_p(d.position), prev_p(d.position), curr_r(d.angle),
		      prev_r(d.angle) {}
	};

	int BodyDef(lua_State *L)
	{
		lua_createtable(L, 0, 16);
		luaR_rawsetinteger(L, "type", b2_staticBody);
		luaR_rawsetvec2(L, "position");
		luaR_rawsetnumber(L, "angle", 0.0f);
		luaR_rawsetvec2(L, "linearVelocity");
		luaR_rawsetnumber(L, "angularVelocity", 0.0f);
		luaR_rawsetnumber(L, "linearDamping", 0.0f);
		luaR_rawsetnumber(L, "angularDamping", 0.0f);
		luaR_rawsetboolean(L, "allowSleep", true);
		luaR_rawsetboolean(L, "awake", true);
		luaR_rawsetboolean(L, "fixedRotation", false);
		luaR_rawsetboolean(L, "bullet", false);
		luaR_rawsetboolean(L, "active", true);
		luaR_rawsetnumber(L, "gravityScale", 1.0f);
		return 1;
	}

	void parse(lua_State *L, b2BodyDef &def)
	{
		def.type = static_cast<b2BodyType>(luaR_getinteger(L, "type"));

		luaR_getfield(L, "position");
		def.position.x = luaR_getnumber(L, "x") / g_ptm_ratio;
		def.position.y = luaR_getnumber(L, "y") / g_ptm_ratio;
		lua_pop(L, 1);

		def.angle = luaR_getnumber(L, "angle");

		luaR_getfield(L, "linearVelocity");
		def.linearVelocity.x = luaR_getnumber(L, "x");
		def.linearVelocity.y = luaR_getnumber(L, "y");
		lua_pop(L, 1);

		def.linearDamping = luaR_getnumber(L, "linearDamping");
		def.angularDamping = luaR_getnumber(L, "angularDamping");
		def.allowSleep = luaR_getboolean(L, "allowSleep");
		def.awake = luaR_getboolean(L, "awake");
		def.fixedRotation = luaR_getboolean(L, "fixedRotation");
		def.bullet = luaR_getboolean(L, "bullet");
		def.active = luaR_getboolean(L, "active");
		def.gravityScale = luaR_getnumber(L, "gravityScale");
	}

	class Body : public Bind<Body>
	{
		friend Bind;

	public:
		Body(lua_State *);
		inline b2Body* get();

	private:
		static int bind(lua_State *);
		static int scale_and_position(lua_State *);

		static int create_fixture(lua_State *);
		static int destroy_fixture(lua_State *);

		static int set_transform(lua_State *);
		static int get_position(lua_State *);
		static int get_angle(lua_State *);
		static int get_world_center(lua_State *);
		static int get_local_center(lua_State *);

		static int set_linear_velocity(lua_State *);
		static int get_linear_velocity(lua_State *);
		static int set_angular_velocity(lua_State *);
		static int get_angular_velocity(lua_State *);

		static int apply_force(lua_State *);
		static int apply_force_to_center(lua_State *);
		static int apply_torque(lua_State *);
		static int apply_linear_impulse(lua_State *);
		static int apply_angular_impulse(lua_State *);

		static int get_linear_damping(lua_State *);
		static int set_linear_damping(lua_State *);
		static int get_angular_damping(lua_State *);
		static int set_angular_damping(lua_State *);

		static int get_gravity_scale(lua_State *);
		static int set_gravity_scale(lua_State *);

		static int set_bullet(lua_State *);
		static int is_bullet(lua_State *);

		static int set_sleeping_allowed(lua_State *);
		static int is_sleeping_allowed(lua_State *);
		static int set_awake(lua_State *);
		static int is_awake(lua_State *);
		static int set_active(lua_State *);
		static int is_active(lua_State *);

		static int set_fixed_rotation(lua_State *);
		static int is_fixed_rotation(lua_State *);

		static int dump(lua_State *);

		b2Body *body_;
	};

	Body::Body(lua_State *L)
	    : body_(static_cast<b2Body*>(lua_touserdata(L, -1))) {}

	b2Body* Body::get()
	{
		return body_;
	}

	int Body::bind(lua_State *L)
	{
		// <b2.Body>:Bind(<rainbow.sprite>)
		Rainbow::Lua::Argument<Rainbow::Lua::Sprite>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		BodyData *data = static_cast<BodyData*>(self->body_->GetUserData());
		Rainbow::Lua::replacetable(L, 2);
		data->sprite =
		    Rainbow::Lua::touserdata<Rainbow::Lua::Sprite>(L, 2)->get();
		b2Vec2 pos = self->body_->GetPosition();
		pos *= g_ptm_ratio;
		data->sprite->set_position(Vec2f(pos.x, pos.y));
		data->sprite->set_rotation(self->body_->GetAngle());
		return 0;
	}

	int Body::scale_and_position(lua_State *L)
	{
		// <b2.Body>:ScaleAndPosition(scale, x, y)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 4);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		const lua_Number scale = lua_tonumber(L, 2);
		const lua_Number x = lua_tonumber(L, 3);
		const lua_Number y = lua_tonumber(L, 4);

		b2BodyDef body_def;
		body_def.type = self->body_->GetType();
		body_def.position.Set(x / g_ptm_ratio, y / g_ptm_ratio);
		body_def.angle = self->body_->GetAngle();
		body_def.linearVelocity = self->body_->GetLinearVelocity();
		body_def.angularVelocity = self->body_->GetAngularVelocity();
		body_def.linearDamping = self->body_->GetLinearDamping();
		body_def.angularDamping = self->body_->GetAngularDamping();
		body_def.allowSleep = self->body_->IsSleepingAllowed();
		body_def.awake = self->body_->IsAwake();
		body_def.fixedRotation = self->body_->IsFixedRotation();
		body_def.bullet = self->body_->IsBullet();
		body_def.active = self->body_->IsActive();
		body_def.userData = self->body_->GetUserData();
		body_def.gravityScale = self->body_->GetGravityScale();

		b2World *world = self->body_->GetWorld();
		b2Body *new_body = world->CreateBody(&body_def);

		b2FixtureDef fixture;
		b2CircleShape shape;
		fixture.shape = &shape;
		for (b2Fixture *f = self->body_->GetFixtureList(); f; f = f->GetNext())
		{
			if (f->GetType() != b2Shape::e_circle)
			{
				LUA_ASSERT(
				    L, f->GetType() == b2Shape::e_circle, "Unsupported shape");
				world->DestroyBody(new_body);
				return 0;
			}
			shape = *static_cast<b2CircleShape*>(f->GetShape());
			shape.m_radius *= scale;
			fixture.userData = f->GetUserData();
			fixture.friction = f->GetFriction();
			fixture.restitution = f->GetRestitution();
			fixture.density = f->GetDensity();
			fixture.isSensor = f->IsSensor();
			fixture.filter = f->GetFilterData();
			new_body->CreateFixture(&fixture);
		}

		// Reset user data
		if (body_def.userData)
		{
			BodyData *d = static_cast<BodyData*>(body_def.userData);
			d->prev_p = body_def.position;
			d->curr_p = body_def.position;
			d->prev_r = body_def.angle;
			d->curr_r = body_def.angle;
			if (d->sprite)
			{
				d->sprite->set_position(Vec2f(x, y));
				d->sprite->set_rotation(body_def.angle);
			}
		}

		// Re-register body
		lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
		lua_pushlightuserdata(L, self->body_);
		lua_rawget(L, -2);
		lua_pushlightuserdata(L, new_body);
		lua_insert(L, -2);
		lua_rawset(L, -3);
		lua_pushlightuserdata(L, self->body_);
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1);

		world->DestroyBody(self->body_);
		self->body_ = new_body;
		return 0;
	}

	int Body::create_fixture(lua_State *L)
	{
		// <b2.Body>:CreateFixture(<b2.FixtureDef> | [<b2.Shape>, density])
		Rainbow::Lua::Argument<void*>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_optional(L, 3);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		b2Fixture *fixture;
		switch (lua_gettop(L))
		{
			case 2: {
				b2FixtureDef def;
				parse(L, def);
				fixture = self->body_->CreateFixture(&def);
				delete def.shape;
				break;
			}
			default: {
				R_ASSERT(lua_gettop(L) > 2, "This shouldn't ever happen");
				const float density = lua_tonumber(L, 3);
				lua_settop(L, 2);
				std::unique_ptr<b2Shape> shape(parse_Shape(L));
				fixture = self->body_->CreateFixture(shape.get(), density);
				break;
			}
		}
		lua_pushlightuserdata(L, fixture);
		return 1;
	}

	int Body::destroy_fixture(lua_State *L)
	{
		// <b2.Body>:DestroyFixture(<b2.Fixture>)
		Rainbow::Lua::Argument<b2Fixture>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->DestroyFixture(
		    static_cast<b2Fixture*>(lua_touserdata(L, 2)));
		return 0;
	}

	int Body::set_transform(lua_State *L)
	{
		// <b2.Body>:SetTransform(x, y, r)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 4);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetTransform(
		    b2Vec2(lua_tonumber(L, 2) / g_ptm_ratio,
		           lua_tonumber(L, 3) / g_ptm_ratio),
		    lua_tonumber(L, 4));
		return 0;
	}

	int Body::get_position(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		const b2Vec2 &pos = self->body_->GetPosition();
		lua_pushnumber(L, pos.x * g_ptm_ratio);
		lua_pushnumber(L, pos.y * g_ptm_ratio);
		return 2;
	}

	int Body::get_angle(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushnumber(L, self->body_->GetAngle());
		return 1;
	}

	int Body::get_world_center(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		const b2Vec2 &center = self->body_->GetWorldCenter();
		lua_pushnumber(L, center.x * g_ptm_ratio);
		lua_pushnumber(L, center.y * g_ptm_ratio);
		return 2;
	}

	int Body::get_local_center(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		const b2Vec2 &center = self->body_->GetLocalCenter();
		lua_pushnumber(L, center.x * g_ptm_ratio);
		lua_pushnumber(L, center.y * g_ptm_ratio);
		return 2;
	}

	int Body::set_linear_velocity(lua_State *L)
	{
		// <b2.Body>:SetLinearVelocity(x, y)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetLinearVelocity(
		    b2Vec2(lua_tonumber(L, 2), lua_tonumber(L, 3)));
		return 0;
	}

	int Body::get_linear_velocity(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		const b2Vec2 &v = self->body_->GetLinearVelocity();
		lua_pushnumber(L, v.x);
		lua_pushnumber(L, v.y);
		return 2;
	}

	int Body::set_angular_velocity(lua_State *L)
	{
		// <b2.Body>:SetAngularVelocity(v)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetAngularVelocity(lua_tonumber(L, 2));
		return 0;
	}

	int Body::get_angular_velocity(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushnumber(L, self->body_->GetAngularVelocity());
		return 1;
	}

	int Body::apply_force(lua_State *L)
	{
		// <b2.Body>:ApplyForce(impulse.x, impulse.y, point.x, point.y, wake)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 4);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 5);
		Rainbow::Lua::Argument<bool>::is_required(L, 6);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->ApplyForce(
		    b2Vec2(lua_tonumber(L, 2), lua_tonumber(L, 3)),
		    b2Vec2(lua_tonumber(L, 4) / g_ptm_ratio,
		           lua_tonumber(L, 5) / g_ptm_ratio),
		    lua_toboolean(L, 6));
		return 0;
	}

	int Body::apply_force_to_center(lua_State *L)
	{
		// <b2.Body>:ApplyForceToCenter(impulse.x, impulse.y, wake)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);
		Rainbow::Lua::Argument<bool>::is_required(L, 4);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->ApplyForceToCenter(
		    b2Vec2(lua_tonumber(L, 2), lua_tonumber(L, 3)),
		    lua_toboolean(L, 4));
		return 0;
	}

	int Body::apply_torque(lua_State *L)
	{
		// <b2.Body>:ApplyTorque(torque, wake)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<bool>::is_required(L, 3);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->ApplyTorque(lua_tonumber(L, 2), lua_toboolean(L, 3));
		return 0;
	}

	int Body::apply_linear_impulse(lua_State *L)
	{
		// <b2.Body>:ApplyLinearImpulse(impulse.x, impulse.y, point.x, point.y, wake)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 4);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 5);
		Rainbow::Lua::Argument<bool>::is_required(L, 6);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->ApplyLinearImpulse(
		    b2Vec2(lua_tonumber(L, 2), lua_tonumber(L, 3)),
		    b2Vec2(lua_tonumber(L, 4) / g_ptm_ratio,
		           lua_tonumber(L, 5) / g_ptm_ratio),
		    lua_toboolean(L, 6));
		return 0;
	}

	int Body::apply_angular_impulse(lua_State *L)
	{
		// <b2.Body>:ApplyAngularImpulse(impulse, wake)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<bool>::is_required(L, 3);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->ApplyAngularImpulse(lua_tonumber(L, 2),
		                                lua_toboolean(L, 3));
		return 0;
	}

	int Body::get_linear_damping(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushnumber(L, self->body_->GetLinearDamping() * g_ptm_ratio);
		return 1;
	}

	int Body::set_linear_damping(lua_State *L)
	{
		// <b2.Body>:SetLinearDamping(damping)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetLinearDamping(lua_tonumber(L, 2) / g_ptm_ratio);
		return 0;
	}

	int Body::get_angular_damping(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushnumber(L, self->body_->GetAngularDamping());
		return 1;
	}

	int Body::set_angular_damping(lua_State *L)
	{
		// <b2.Body>:SetAngularDamping(damping)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetAngularDamping(lua_tonumber(L, 2));
		return 0;
	}

	int Body::get_gravity_scale(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushnumber(L, self->body_->GetGravityScale());
		return 1;
	}

	int Body::set_gravity_scale(lua_State *L)
	{
		// <b2.Body>:SetGravityScale(scale)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetGravityScale(lua_tonumber(L, 2));
		return 0;
	}

	int Body::set_bullet(lua_State *L)
	{
		// <b2.Body>:SetBullet(bool)
		Rainbow::Lua::Argument<bool>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetBullet(lua_toboolean(L, 2));
		return 0;
	}

	int Body::is_bullet(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->body_->IsBullet());
		return 1;
	}

	int Body::set_sleeping_allowed(lua_State *L)
	{
		// <b2.Body>:SetSleepingAllowed(bool)
		Rainbow::Lua::Argument<bool>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetSleepingAllowed(lua_toboolean(L, 2));
		return 0;
	}

	int Body::is_sleeping_allowed(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->body_->IsSleepingAllowed());
		return 1;
	}

	int Body::set_awake(lua_State *L)
	{
		// <b2.Body>:SetAwake(bool)
		Rainbow::Lua::Argument<bool>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetAwake(lua_toboolean(L, 2));
		return 0;
	}

	int Body::is_awake(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->body_->IsAwake());
		return 1;
	}

	int Body::set_active(lua_State *L)
	{
		// <b2.Body>:SetActive(bool)
		Rainbow::Lua::Argument<bool>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetActive(lua_toboolean(L, 2));
		return 0;
	}

	int Body::is_active(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->body_->IsActive());
		return 0;
	}

	int Body::set_fixed_rotation(lua_State *L)
	{
		// <b2.Body>:SetFixedRotation(bool)
		Rainbow::Lua::Argument<bool>::is_required(L, 2);

		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->SetFixedRotation(lua_toboolean(L, 2));
		return 0;
	}

	int Body::is_fixed_rotation(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->body_->IsFixedRotation());
		return 0;
	}

	int Body::dump(lua_State *L)
	{
		Body *self = Bind::self(L);
		if (!self)
			return 0;

		self->body_->Dump();
		return 0;
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::Lua::Body;

	template<>
	const char Body::Bind::class_name[] = "Body";

	template<>
	const bool Body::Bind::is_constructible = true;

	template<>
	const luaL_Reg Body::Bind::functions[] = {
		{ "Bind",                 &Body::bind },
		{ "ScaleAndPosition",     &Body::scale_and_position },
		{ "CreateFixture",        &Body::create_fixture },
		{ "DestroyFixture",       &Body::destroy_fixture },
		{ "SetTransform",         &Body::set_transform },
		{ "GetPosition",          &Body::get_position },
		{ "GetAngle",             &Body::get_angle },
		{ "GetWorldCenter",       &Body::get_world_center },
		{ "GetLocalCenter",       &Body::get_local_center },
		{ "SetLinearVelocity",    &Body::set_linear_velocity },
		{ "GetLinearVelocity",    &Body::get_linear_velocity },
		{ "SetAngularVelocity",   &Body::set_angular_velocity },
		{ "GetAngularVelocity",   &Body::get_angular_velocity },
		{ "ApplyForce",           &Body::apply_force },
		{ "ApplyForceToCenter",   &Body::apply_force_to_center },
		{ "ApplyTorque",          &Body::apply_torque },
		{ "ApplyLinearImpulse",   &Body::apply_linear_impulse },
		{ "ApplyAngularImpulse",  &Body::apply_angular_impulse },
		{ "GetLinearDamping",     &Body::get_linear_damping },
		{ "SetLinearDamping",     &Body::set_linear_damping },
		{ "GetAngularDamping",    &Body::get_angular_damping },
		{ "SetAngularDamping",    &Body::set_angular_damping },
		{ "GetGravityScale",      &Body::get_gravity_scale },
		{ "SetGravityScale",      &Body::set_gravity_scale },
		{ "SetBullet",            &Body::set_bullet },
		{ "IsBullet",             &Body::is_bullet },
		{ "SetSleepingAllowed",   &Body::set_sleeping_allowed },
		{ "IsSleepingAllowed",    &Body::is_sleeping_allowed },
		{ "SetAwake",             &Body::set_awake },
		{ "IsAwake",              &Body::is_awake },
		{ "SetActive",            &Body::set_active },
		{ "IsActive",             &Body::is_active },
		{ "SetFixedRotation",     &Body::set_fixed_rotation },
		{ "IsFixedRotation",      &Body::is_fixed_rotation },
		{ "Dump",                 &Body::dump },
		{ nullptr,                nullptr }
	};
} NS_RAINBOW_LUA_END

#endif
