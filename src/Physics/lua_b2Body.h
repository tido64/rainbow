// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Sprite.h"
#include "Lua/lua_Sprite.h"

namespace b2
{
	namespace Lua
	{
		struct BodyData
		{
			float curr_r, prev_r;
			Sprite *sprite;
			b2Vec2 curr_p, prev_p;

			BodyData(const b2BodyDef &d) :
				curr_r(d.angle), prev_r(d.angle), sprite(nullptr),
				curr_p(d.position), prev_p(d.position) { }
		};

		int BodyDef(lua_State *L)
		{
			lua_createtable(L, 0, 16);

			luaR_rawsetfield(L, lua_pushinteger, b2_staticBody, "type");

			lua_pushliteral(L, "position");
			lua_createtable(L, 0, 2);
			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "x");
			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "y");
			lua_rawset(L, -3);

			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "angle");

			lua_pushliteral(L, "linearVelocity");
			lua_createtable(L, 0, 2);
			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "x");
			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "y");
			lua_rawset(L, -3);

			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "angularVelocity");
			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "linearDamping");
			luaR_rawsetfield(L, lua_pushnumber, 0.0f, "angularDamping");
			luaR_rawsetfield(L, lua_pushboolean, true, "allowSleep");
			luaR_rawsetfield(L, lua_pushboolean, true, "awake");
			luaR_rawsetfield(L, lua_pushboolean, false, "fixedRotation");
			luaR_rawsetfield(L, lua_pushboolean, false, "bullet");
			luaR_rawsetfield(L, lua_pushboolean, true, "active");
			luaR_rawsetfield(L, lua_pushnumber, 1.0f, "gravityScale");

			return 1;
		}

		void parse_BodyDef(lua_State *L, b2BodyDef &def)
		{
			const char type[] = "BodyDef";
			static_cast<void>(type);

			luaR_rawgetfield(L, "type", type);
			def.type = static_cast<b2BodyType>(luaR_tointeger(L, -1));
			lua_pop(L, 1);

			luaR_rawgetfield(L, "position", type);
			lua_pushliteral(L, "x");
			lua_rawget(L, -2);
			def.position.x = luaR_tonumber(L, -1) / ptm_ratio;
			lua_pushliteral(L, "y");
			lua_rawget(L, -3);
			def.position.y = luaR_tonumber(L, -1) / ptm_ratio;
			lua_pop(L, 3);

			luaR_rawgetfield(L, "angle", type);
			def.angle = luaR_tonumber(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "linearVelocity", type);
			lua_pushliteral(L, "x");
			lua_rawget(L, -2);
			def.linearVelocity.x = luaR_tonumber(L, -1);
			lua_pushliteral(L, "y");
			lua_rawget(L, -3);
			def.linearVelocity.y = luaR_tonumber(L, -1);
			lua_pop(L, 3);

			luaR_rawgetfield(L, "linearDamping", type);
			def.linearDamping = luaR_tonumber(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "angularDamping", type);
			def.angularDamping = luaR_tonumber(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "allowSleep", type);
			def.allowSleep = lua_toboolean(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "awake", type);
			def.awake = lua_toboolean(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "fixedRotation", type);
			def.fixedRotation = lua_toboolean(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "bullet", type);
			def.bullet = lua_toboolean(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "active", type);
			def.active = lua_toboolean(L, -1);
			lua_pop(L, 1);

			luaR_rawgetfield(L, "gravityScale", type);
			def.gravityScale = luaR_tonumber(L, -1);
			lua_pop(L, 1);
		}

		class Body :
			public Rainbow::Lua::Bind<Body , b2Body, Rainbow::Lua::kBindTypeWeak>
		{
			friend class Rainbow::Lua::Bind<Body , b2Body, Rainbow::Lua::kBindTypeWeak>;

		public:
			Body(lua_State *);

		private:
			int bind(lua_State *);
			int scale_and_position(lua_State *);

			int create_fixture(lua_State *);
			int destroy_fixture(lua_State *);

			int set_transform(lua_State *);
			int get_position(lua_State *);
			int get_angle(lua_State *);
			int get_world_center(lua_State *);
			int get_local_center(lua_State *);

			int set_linear_velocity(lua_State *);
			int get_linear_velocity(lua_State *);
			int set_angular_velocity(lua_State *);
			int get_angular_velocity(lua_State *);

			int apply_force(lua_State *);
			int apply_force_to_center(lua_State *);
			int apply_torque(lua_State *);
			int apply_linear_impulse(lua_State *);
			int apply_angular_impulse(lua_State *);

			int get_linear_damping(lua_State *);
			int set_linear_damping(lua_State *);
			int get_angular_damping(lua_State *);
			int set_angular_damping(lua_State *);

			int get_gravity_scale(lua_State *);
			int set_gravity_scale(lua_State *);

			int set_bullet(lua_State *);
			int is_bullet(lua_State *);

			int set_sleeping_allowed(lua_State *);
			int is_sleeping_allowed(lua_State *);
			int set_awake(lua_State *);
			int is_awake(lua_State *);
			int set_active(lua_State *);
			int is_active(lua_State *);

			int set_fixed_rotation(lua_State *);
			int is_fixed_rotation(lua_State *);

			int dump(lua_State *);
		};

		Body::Body(lua_State *L) :
			Bind(static_cast<b2Body*>(lua_touserdata(L, -1))) { }

		int Body::bind(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:Bind(<rainbow.sprite>)");

			BodyData *data = static_cast<BodyData*>(this->ptr->GetUserData());
			data->sprite = Rainbow::Lua::wrapper<Rainbow::Lua::Sprite>(L)->get();
			b2Vec2 pos = this->ptr->GetPosition();
			pos *= ptm_ratio;
			data->sprite->set_position(Vec2f(pos.x, pos.y));
			data->sprite->set_rotation(this->ptr->GetAngle());
			return 0;
		}

		int Body::scale_and_position(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3, "<b2.Body>:ScaleAndPosition(scale, x, y)");

			const lua_Number scale = luaR_tonumber(L, 1);
			const lua_Number x = luaR_tonumber(L, 2);
			const lua_Number y = luaR_tonumber(L, 3);

			b2BodyDef body_def;
			body_def.type = this->ptr->GetType();
			body_def.position.Set(x / ptm_ratio, y / ptm_ratio);
			body_def.angle = this->ptr->GetAngle();
			body_def.linearVelocity = this->ptr->GetLinearVelocity();
			body_def.angularVelocity = this->ptr->GetAngularVelocity();
			body_def.linearDamping = this->ptr->GetLinearDamping();
			body_def.angularDamping = this->ptr->GetAngularDamping();
			body_def.allowSleep = this->ptr->IsSleepingAllowed();
			body_def.awake = this->ptr->IsAwake();
			body_def.fixedRotation = this->ptr->IsFixedRotation();
			body_def.bullet = this->ptr->IsBullet();
			body_def.active = this->ptr->IsActive();
			body_def.userData = this->ptr->GetUserData();
			body_def.gravityScale = this->ptr->GetGravityScale();

			b2World *world = this->ptr->GetWorld();
			b2Body *new_body = world->CreateBody(&body_def);

			b2FixtureDef fixture;
			b2CircleShape shape;
			fixture.shape = &shape;
			for (b2Fixture *f = this->ptr->GetFixtureList(); f; f = f->GetNext())
			{
				if (f->GetType() != b2Shape::e_circle)
				{
					LUA_ASSERT(f->GetType() == b2Shape::e_circle, "Unsupported shape");
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
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(this->ptr));
			lua_rawget(L, -2);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(new_body));
			lua_insert(L, -2);
			lua_rawset(L, -3);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(this->ptr));
			lua_pushnil(L);
			lua_rawset(L, -3);
			lua_pop(L, 1);

			world->DestroyBody(this->ptr);
			this->ptr = new_body;
			return 0;
		}

		int Body::create_fixture(lua_State *L)
		{
			b2Fixture *fixture = nullptr;
			switch (lua_gettop(L))
			{
				case 1:
					{
						b2FixtureDef def;
						parse_FixtureDef(L, def);
						fixture = this->ptr->CreateFixture(&def);
						delete def.shape;
					}
					break;
				case 2:
					{
						const float density = luaR_tonumber(L, 2);
						lua_pop(L, 1);
						std::unique_ptr<b2Shape> shape(parse_Shape(L));
						fixture = this->ptr->CreateFixture(shape.get(), density);
					}
					break;
				default:
					LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2,
					           "<b2.Body>:CreateFixture(b2.FixtureDef | b2.Shape, density)");
					break;
			}
			lua_pushlightuserdata(L, fixture);
			return 1;
		}

		int Body::destroy_fixture(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:DestroyFixture(b2.Fixture)");

			this->ptr->DestroyFixture(static_cast<b2Fixture*>(lua_touserdata(L, 1)));
			return 0;
		}

		int Body::set_transform(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3, "<b2.Body>:SetTransform(x, y, r)");

			this->ptr->SetTransform(
					b2Vec2(luaR_tonumber(L, 1) / ptm_ratio, luaR_tonumber(L, 2) / ptm_ratio),
					luaR_tonumber(L, 3));
			return 0;
		}

		int Body::get_position(lua_State *L)
		{
			const b2Vec2 &pos = this->ptr->GetPosition();
			lua_pushnumber(L, pos.x * ptm_ratio);
			lua_pushnumber(L, pos.y * ptm_ratio);
			return 2;
		}

		int Body::get_angle(lua_State *L)
		{
			lua_pushnumber(L, this->ptr->GetAngle());
			return 1;
		}

		int Body::get_world_center(lua_State *L)
		{
			const b2Vec2 &center = this->ptr->GetWorldCenter();
			lua_pushnumber(L, center.x * ptm_ratio);
			lua_pushnumber(L, center.y * ptm_ratio);
			return 2;
		}

		int Body::get_local_center(lua_State *L)
		{
			const b2Vec2 &center = this->ptr->GetLocalCenter();
			lua_pushnumber(L, center.x * ptm_ratio);
			lua_pushnumber(L, center.y * ptm_ratio);
			return 2;
		}

		int Body::set_linear_velocity(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<b2.Body>:SetLinearVelocity(x, y)");

			this->ptr->SetLinearVelocity(
					b2Vec2(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));
			return 0;
		}

		int Body::get_linear_velocity(lua_State *L)
		{
			const b2Vec2 &v = this->ptr->GetLinearVelocity();
			lua_pushnumber(L, v.x);
			lua_pushnumber(L, v.y);
			return 2;
		}

		int Body::set_angular_velocity(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetAngularVelocity(v)");

			this->ptr->SetAngularVelocity(luaR_tonumber(L, 1));
			return 0;
		}

		int Body::get_angular_velocity(lua_State *L)
		{
			lua_pushnumber(L, this->ptr->GetAngularVelocity());
			return 1;
		}

		int Body::apply_force(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 5,
			           "<b2.Body>:ApplyForce(impulse.x, impulse.y, point.x, point.y, wake)");

			this->ptr->ApplyForce(
					b2Vec2(luaR_tonumber(L, 1), luaR_tonumber(L, 2)),
					b2Vec2(luaR_tonumber(L, 3) / ptm_ratio, luaR_tonumber(L, 4) / ptm_ratio),
					lua_toboolean(L, 5));
			return 0;
		}

		int Body::apply_force_to_center(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3,
			           "<b2.Body>:ApplyForceToCenter(impulse.x, impulse.y, wake)");

			this->ptr->ApplyForceToCenter(
					b2Vec2(luaR_tonumber(L, 1), luaR_tonumber(L, 2)),
					lua_toboolean(L, 3));
			return 0;
		}

		int Body::apply_torque(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2,
			           "<b2.Body>:ApplyTorque(torque, wake)");

			this->ptr->ApplyTorque(luaR_tonumber(L, 1), lua_toboolean(L, 2));
			return 0;
		}

		int Body::apply_linear_impulse(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 5,
			           "<b2.Body>:ApplyLinearImpulse(impulse.x, impulse.y, point.x, point.y, wake)");

			this->ptr->ApplyLinearImpulse(
					b2Vec2(luaR_tonumber(L, 1), luaR_tonumber(L, 2)),
					b2Vec2(luaR_tonumber(L, 3) / ptm_ratio, luaR_tonumber(L, 4) / ptm_ratio),
					lua_toboolean(L, 5));
			return 0;
		}

		int Body::apply_angular_impulse(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2,
			           "<b2.Body>:ApplyAngularImpulse(impulse, wake)");

			this->ptr->ApplyAngularImpulse(luaR_tonumber(L, 1), lua_toboolean(L, 2));
			return 0;
		}

		int Body::get_linear_damping(lua_State *L)
		{
			lua_pushnumber(L, this->ptr->GetLinearDamping() * ptm_ratio);
			return 1;
		}

		int Body::set_linear_damping(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:SetLinearDamping(damping)");

			this->ptr->SetLinearDamping(luaR_tonumber(L, 1) / ptm_ratio);
			return 0;
		}

		int Body::get_angular_damping(lua_State *L)
		{
			lua_pushnumber(L, this->ptr->GetAngularDamping());
			return 1;
		}

		int Body::set_angular_damping(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:SetAngularDamping(damping)");

			this->ptr->SetAngularDamping(luaR_tonumber(L, 1));
			return 0;
		}

		int Body::get_gravity_scale(lua_State *L)
		{
			lua_pushnumber(L, this->ptr->GetGravityScale());
			return 1;
		}

		int Body::set_gravity_scale(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetGravityScale(scale)");

			this->ptr->SetGravityScale(luaR_tonumber(L, 1));
			return 0;
		}

		int Body::set_bullet(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetBullet(flag)");

			this->ptr->SetBullet(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_bullet(lua_State *L)
		{
			lua_pushboolean(L, this->ptr->IsBullet());
			return 1;
		}

		int Body::set_sleeping_allowed(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:SetSleepingAllowed(flag)");

			this->ptr->SetSleepingAllowed(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_sleeping_allowed(lua_State *L)
		{
			lua_pushboolean(L, this->ptr->IsSleepingAllowed());
			return 1;
		}

		int Body::set_awake(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetAwake(flag)");

			this->ptr->SetAwake(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_awake(lua_State *L)
		{
			lua_pushboolean(L, this->ptr->IsAwake());
			return 1;
		}

		int Body::set_active(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetActive(flag)");

			this->ptr->SetActive(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_active(lua_State *L)
		{
			lua_pushboolean(L, this->ptr->IsActive());
			return 0;
		}

		int Body::set_fixed_rotation(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetFixedRotation(flag)");

			this->ptr->SetFixedRotation(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_fixed_rotation(lua_State *L)
		{
			lua_pushboolean(L, this->ptr->IsFixedRotation());
			return 0;
		}

		int Body::dump(lua_State *)
		{
			this->ptr->Dump();
			return 0;
		}
	}
}

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<b2::Lua::Body, b2Body, kBindTypeWeak> b2LuaBody;

		template<>
		const char b2LuaBody::class_name[] = "Body";

		template<>
		const Method<b2::Lua::Body> b2LuaBody::methods[] = {
			{ "Bind",                 &b2::Lua::Body::bind },
			{ "ScaleAndPosition",     &b2::Lua::Body::scale_and_position },
			{ "CreateFixture",        &b2::Lua::Body::create_fixture },
			{ "DestroyFixture",       &b2::Lua::Body::destroy_fixture },
			{ "SetTransform",         &b2::Lua::Body::set_transform },
			{ "GetPosition",          &b2::Lua::Body::get_position },
			{ "GetAngle",             &b2::Lua::Body::get_angle },
			{ "GetWorldCenter",       &b2::Lua::Body::get_world_center },
			{ "GetLocalCenter",       &b2::Lua::Body::get_local_center },
			{ "SetLinearVelocity",    &b2::Lua::Body::set_linear_velocity },
			{ "GetLinearVelocity",    &b2::Lua::Body::get_linear_velocity },
			{ "SetAngularVelocity",   &b2::Lua::Body::set_angular_velocity },
			{ "GetAngularVelocity",   &b2::Lua::Body::get_angular_velocity },
			{ "ApplyForce",           &b2::Lua::Body::apply_force },
			{ "ApplyForceToCenter",   &b2::Lua::Body::apply_force_to_center },
			{ "ApplyTorque",          &b2::Lua::Body::apply_torque },
			{ "ApplyLinearImpulse",   &b2::Lua::Body::apply_linear_impulse },
			{ "ApplyAngularImpulse",  &b2::Lua::Body::apply_angular_impulse },
			{ "GetLinearDamping",     &b2::Lua::Body::get_linear_damping },
			{ "SetLinearDamping",     &b2::Lua::Body::set_linear_damping },
			{ "GetAngularDamping",    &b2::Lua::Body::get_angular_damping },
			{ "SetAngularDamping",    &b2::Lua::Body::set_angular_damping },
			{ "GetGravityScale",      &b2::Lua::Body::get_gravity_scale },
			{ "SetGravityScale",      &b2::Lua::Body::set_gravity_scale },
			{ "SetBullet",            &b2::Lua::Body::set_bullet },
			{ "IsBullet",             &b2::Lua::Body::is_bullet },
			{ "SetSleepingAllowed",   &b2::Lua::Body::set_sleeping_allowed },
			{ "IsSleepingAllowed",    &b2::Lua::Body::is_sleeping_allowed },
			{ "SetAwake",             &b2::Lua::Body::set_awake },
			{ "IsAwake",              &b2::Lua::Body::is_awake },
			{ "SetActive",            &b2::Lua::Body::set_active },
			{ "IsActive",             &b2::Lua::Body::is_active },
			{ "SetFixedRotation",     &b2::Lua::Body::set_fixed_rotation },
			{ "IsFixedRotation",      &b2::Lua::Body::is_fixed_rotation },
			{ "Dump",                 &b2::Lua::Body::dump },
			{ nullptr,                nullptr }
		};
	}
}
