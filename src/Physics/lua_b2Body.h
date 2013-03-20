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

			lua_rawsetfield(L, lua_pushinteger, b2_staticBody, "type");

			lua_pushliteral(L, "position");
			lua_createtable(L, 0, 2);
			lua_rawsetfield(L, lua_pushnumber, 0.0f, "x");
			lua_rawsetfield(L, lua_pushnumber, 0.0f, "y");
			lua_rawset(L, -3);

			lua_rawsetfield(L, lua_pushnumber, 0.0f, "angle");

			lua_pushliteral(L, "linearVelocity");
			lua_createtable(L, 0, 2);
			lua_rawsetfield(L, lua_pushnumber, 0.0f, "x");
			lua_rawsetfield(L, lua_pushnumber, 0.0f, "y");
			lua_rawset(L, -3);

			lua_rawsetfield(L, lua_pushnumber, 0.0f, "angularVelocity");
			lua_rawsetfield(L, lua_pushnumber, 0.0f, "linearDamping");
			lua_rawsetfield(L, lua_pushnumber, 0.0f, "angularDamping");
			lua_rawsetfield(L, lua_pushboolean, true, "allowSleep");
			lua_rawsetfield(L, lua_pushboolean, true, "awake");
			lua_rawsetfield(L, lua_pushboolean, false, "fixedRotation");
			lua_rawsetfield(L, lua_pushboolean, false, "bullet");
			lua_rawsetfield(L, lua_pushboolean, true, "active");
			lua_rawsetfield(L, lua_pushnumber, 1.0f, "gravityScale");

			return 1;
		}

		void parse_BodyDef(lua_State *L, b2BodyDef &def)
		{
			const char type[] = "BodyDef";
			static_cast<void>(type);

			lua_rawgetfield(L, "type", type);
			def.type = static_cast<b2BodyType>(lua_tointeger(L, -1));
			lua_pop(L, 1);

			lua_rawgetfield(L, "position", type);
			lua_pushliteral(L, "x");
			lua_rawget(L, -2);
			def.position.x = lua_tonumber(L, -1) / ptm_ratio;
			lua_pushliteral(L, "y");
			lua_rawget(L, -3);
			def.position.y = lua_tonumber(L, -1) / ptm_ratio;
			lua_pop(L, 3);

			lua_rawgetfield(L, "angle", type);
			def.angle = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "linearVelocity", type);
			lua_pushliteral(L, "x");
			lua_rawget(L, -2);
			def.linearVelocity.x = lua_tonumber(L, -1);
			lua_pushliteral(L, "y");
			lua_rawget(L, -3);
			def.linearVelocity.y = lua_tonumber(L, -1);
			lua_pop(L, 3);

			lua_rawgetfield(L, "linearDamping", type);
			def.linearDamping = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "angularDamping", type);
			def.angularDamping = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "allowSleep", type);
			def.allowSleep = lua_toboolean(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "awake", type);
			def.awake = lua_toboolean(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "fixedRotation", type);
			def.fixedRotation = lua_toboolean(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "bullet", type);
			def.bullet = lua_toboolean(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "active", type);
			def.active = lua_toboolean(L, -1);
			lua_pop(L, 1);

			lua_rawgetfield(L, "gravityScale", type);
			def.gravityScale = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}

		struct Body
		{
			static const char class_name[];
			static const Rainbow::Lua::Method<Body> methods[];

			b2Body *body;

			Body(lua_State *);

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

		const char Body::class_name[] = "Body";
		const Rainbow::Lua::Method<Body> Body::methods[] = {
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

		Body::Body(lua_State *L) :
			body(static_cast<b2Body*>(lua_touserdata(L, -1))) { }

		int Body::bind(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:Bind(<rainbow.sprite>)");

			BodyData *data = static_cast<BodyData*>(this->body->GetUserData());
			data->sprite = Rainbow::Lua::wrapper<Rainbow::Lua::Sprite>(L)->raw_ptr();
			b2Vec2 pos = this->body->GetPosition();
			pos *= ptm_ratio;
			data->sprite->set_position(Vec2f(pos.x, pos.y));
			data->sprite->set_rotation(this->body->GetAngle());
			return 0;
		}

		int Body::scale_and_position(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3, "<b2.Body>:ScaleAndPosition(scale, x, y)");

			const lua_Number scale = lua_tonumber(L, 1);
			const lua_Number x = lua_tonumber(L, 2);
			const lua_Number y = lua_tonumber(L, 3);

			b2BodyDef body_def;
			body_def.type = this->body->GetType();
			body_def.position.Set(x / ptm_ratio, y / ptm_ratio);
			body_def.angle = this->body->GetAngle();
			body_def.linearVelocity = this->body->GetLinearVelocity();
			body_def.angularVelocity = this->body->GetAngularVelocity();
			body_def.linearDamping = this->body->GetLinearDamping();
			body_def.angularDamping = this->body->GetAngularDamping();
			body_def.allowSleep = this->body->IsSleepingAllowed();
			body_def.awake = this->body->IsAwake();
			body_def.fixedRotation = this->body->IsFixedRotation();
			body_def.bullet = this->body->IsBullet();
			body_def.active = this->body->IsActive();
			body_def.userData = this->body->GetUserData();
			body_def.gravityScale = this->body->GetGravityScale();

			b2World *world = this->body->GetWorld();
			b2Body *new_body = world->CreateBody(&body_def);

			b2FixtureDef fixture;
			b2CircleShape shape;
			fixture.shape = &shape;
			for (b2Fixture *f = this->body->GetFixtureList(); f; f = f->GetNext())
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
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(this->body));
			lua_rawget(L, -2);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(new_body));
			lua_insert(L, -2);
			lua_rawset(L, -3);
			lua_pushinteger(L, reinterpret_cast<lua_Integer>(this->body));
			lua_pushnil(L);
			lua_rawset(L, -3);
			lua_pop(L, 1);

			world->DestroyBody(this->body);
			this->body = new_body;
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
						fixture = this->body->CreateFixture(&def);
						delete def.shape;
					}
					break;
				case 2:
					{
						const float density = lua_tonumber(L, 2);
						lua_pop(L, 1);
						b2Shape *shape = parse_Shape(L);
						fixture = this->body->CreateFixture(shape, density);
						delete shape;
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

			this->body->DestroyFixture(static_cast<b2Fixture*>(lua_touserdata(L, 1)));
			return 0;
		}

		int Body::set_transform(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3, "<b2.Body>:SetTransform(x, y, r)");

			this->body->SetTransform(
					b2Vec2(lua_tonumber(L, 1) / ptm_ratio, lua_tonumber(L, 2) / ptm_ratio),
					lua_tonumber(L, 3));
			return 0;
		}

		int Body::get_position(lua_State *L)
		{
			const b2Vec2 &pos = this->body->GetPosition();
			lua_pushnumber(L, pos.x * ptm_ratio);
			lua_pushnumber(L, pos.y * ptm_ratio);
			return 2;
		}

		int Body::get_angle(lua_State *L)
		{
			lua_pushnumber(L, this->body->GetAngle());
			return 1;
		}

		int Body::get_world_center(lua_State *L)
		{
			const b2Vec2 &center = this->body->GetWorldCenter();
			lua_pushnumber(L, center.x * ptm_ratio);
			lua_pushnumber(L, center.y * ptm_ratio);
			return 2;
		}

		int Body::get_local_center(lua_State *L)
		{
			const b2Vec2 &center = this->body->GetLocalCenter();
			lua_pushnumber(L, center.x * ptm_ratio);
			lua_pushnumber(L, center.y * ptm_ratio);
			return 2;
		}

		int Body::set_linear_velocity(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<b2.Body>:SetLinearVelocity(x, y)");

			this->body->SetLinearVelocity(
					b2Vec2(lua_tonumber(L, 1), lua_tonumber(L, 2)));
			return 0;
		}

		int Body::get_linear_velocity(lua_State *L)
		{
			const b2Vec2 &v = this->body->GetLinearVelocity();
			lua_pushnumber(L, v.x);
			lua_pushnumber(L, v.y);
			return 2;
		}

		int Body::set_angular_velocity(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetAngularVelocity(v)");

			this->body->SetAngularVelocity(lua_tonumber(L, 1));
			return 0;
		}

		int Body::get_angular_velocity(lua_State *L)
		{
			lua_pushnumber(L, this->body->GetAngularVelocity());
			return 1;
		}

		int Body::apply_force(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 5,
			           "<b2.Body>:ApplyForce(impulse.x, impulse.y, point.x, point.y, wake)");

			this->body->ApplyForce(
					b2Vec2(lua_tonumber(L, 1), lua_tonumber(L, 2)),
					b2Vec2(lua_tonumber(L, 3) / ptm_ratio, lua_tonumber(L, 4) / ptm_ratio),
					lua_toboolean(L, 5));
			return 0;
		}

		int Body::apply_force_to_center(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3,
			           "<b2.Body>:ApplyForceToCenter(impulse.x, impulse.y, wake)");

			this->body->ApplyForceToCenter(
					b2Vec2(lua_tonumber(L, 1), lua_tonumber(L, 2)),
					lua_toboolean(L, 3));
			return 0;
		}

		int Body::apply_torque(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2,
			           "<b2.Body>:ApplyTorque(torque, wake)");

			this->body->ApplyTorque(lua_tonumber(L, 1), lua_toboolean(L, 2));
			return 0;
		}

		int Body::apply_linear_impulse(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 5,
			           "<b2.Body>:ApplyLinearImpulse(impulse.x, impulse.y, point.x, point.y, wake)");

			this->body->ApplyLinearImpulse(
					b2Vec2(lua_tonumber(L, 1), lua_tonumber(L, 2)),
					b2Vec2(lua_tonumber(L, 3) / ptm_ratio, lua_tonumber(L, 4) / ptm_ratio),
					lua_toboolean(L, 5));
			return 0;
		}

		int Body::apply_angular_impulse(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2,
			           "<b2.Body>:ApplyAngularImpulse(impulse, wake)");

			this->body->ApplyAngularImpulse(lua_tonumber(L, 1), lua_toboolean(L, 2));
			return 0;
		}

		int Body::get_linear_damping(lua_State *L)
		{
			lua_pushnumber(L, this->body->GetLinearDamping() * ptm_ratio);
			return 1;
		}

		int Body::set_linear_damping(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:SetLinearDamping(damping)");

			this->body->SetLinearDamping(lua_tonumber(L, 1) / ptm_ratio);
			return 0;
		}

		int Body::get_angular_damping(lua_State *L)
		{
			lua_pushnumber(L, this->body->GetAngularDamping());
			return 1;
		}

		int Body::set_angular_damping(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:SetAngularDamping(damping)");

			this->body->SetAngularDamping(lua_tonumber(L, 1));
			return 0;
		}

		int Body::get_gravity_scale(lua_State *L)
		{
			lua_pushnumber(L, this->body->GetGravityScale());
			return 1;
		}

		int Body::set_gravity_scale(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetGravityScale(scale)");

			this->body->SetGravityScale(lua_tonumber(L, 1));
			return 0;
		}

		int Body::set_bullet(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetBullet(flag)");

			this->body->SetBullet(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_bullet(lua_State *L)
		{
			lua_pushboolean(L, this->body->IsBullet());
			return 1;
		}

		int Body::set_sleeping_allowed(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1,
			           "<b2.Body>:SetSleepingAllowed(flag)");

			this->body->SetSleepingAllowed(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_sleeping_allowed(lua_State *L)
		{
			lua_pushboolean(L, this->body->IsSleepingAllowed());
			return 1;
		}

		int Body::set_awake(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetAwake(flag)");

			this->body->SetAwake(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_awake(lua_State *L)
		{
			lua_pushboolean(L, this->body->IsAwake());
			return 1;
		}

		int Body::set_active(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetActive(flag)");

			this->body->SetActive(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_active(lua_State *L)
		{
			lua_pushboolean(L, this->body->IsActive());
			return 0;
		}

		int Body::set_fixed_rotation(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.Body>:SetFixedRotation(flag)");

			this->body->SetFixedRotation(lua_toboolean(L, 1));
			return 0;
		}

		int Body::is_fixed_rotation(lua_State *L)
		{
			lua_pushboolean(L, this->body->IsFixedRotation());
			return 0;
		}

		int Body::dump(lua_State *)
		{
			this->body->Dump();
			return 0;
		}
	}
}
