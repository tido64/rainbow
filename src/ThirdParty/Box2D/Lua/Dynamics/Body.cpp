// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Dynamics/Body.h"

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>

#include "Lua/lua_Sprite.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Fixture.h"
#include "ThirdParty/Box2D/StableWorld.h"

class IDrawable;

NS_B2_LUA_BEGIN
{
    int BodyDef(lua_State* L)
    {
        lua_createtable(L, 0, 14);
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

    b2BodyDef GetBodyDef(lua_State* L)
    {
        b2BodyDef bd;
        bd.type = static_cast<b2BodyType>(luaR_getinteger(L, "type"));

        luaR_getfield(L, "position");
        bd.position.x = luaR_getnumber(L, "x");
        bd.position.y = luaR_getnumber(L, "y");
        lua_pop(L, 1);

        bd.angle = luaR_getnumber(L, "angle");

        luaR_getfield(L, "linearVelocity");
        bd.linearVelocity.x = luaR_getnumber(L, "x");
        bd.linearVelocity.y = luaR_getnumber(L, "y");
        lua_pop(L, 1);

        bd.linearDamping = luaR_getnumber(L, "linearDamping");
        bd.angularDamping = luaR_getnumber(L, "angularDamping");
        bd.allowSleep = luaR_getboolean(L, "allowSleep");
        bd.awake = luaR_getboolean(L, "awake");
        bd.fixedRotation = luaR_getboolean(L, "fixedRotation");
        bd.bullet = luaR_getboolean(L, "bullet");
        bd.active = luaR_getboolean(L, "active");
        bd.gravityScale = luaR_getnumber(L, "gravityScale");

        return bd;
    }

    constexpr bool Body::is_constructible;

    const char Body::class_name[] = "b2Body";

    const luaL_Reg Body::functions[]{
        {"Bind",                 &Body::Bind},
        {"CreateFixture",        &Body::CreateFixture},
        {"DestroyFixture",       &Body::DestroyFixture},
        {"SetTransform",         &Body::SetTransform},
        {"GetPosition",          &Body::GetPosition},
        {"GetAngle",             &Body::GetAngle},
        {"GetWorldCenter",       &Body::GetWorldCenter},
        {"GetLocalCenter",       &Body::GetLocalCenter},
        {"SetLinearVelocity",    &Body::SetLinearVelocity},
        {"GetLinearVelocity",    &Body::GetLinearVelocity},
        {"SetAngularVelocity",   &Body::SetAngularVelocity},
        {"GetAngularVelocity",   &Body::GetAngularVelocity},
        {"ApplyForce",           &Body::ApplyForce},
        {"ApplyForceToCenter",   &Body::ApplyForceToCenter},
        {"ApplyTorque",          &Body::ApplyTorque},
        {"ApplyLinearImpulse",   &Body::ApplyLinearImpulse},
        {"ApplyAngularImpulse",  &Body::ApplyAngularImpulse},
        {"GetMass",              &Body::GetMass},
        {"GetInertia",           &Body::GetInertia},
        {"GetMassData",          &Body::GetMassData},
        {"SetMassData",          &Body::SetMassData},
        {"ResetMassData",        &Body::ResetMassData},
        {"GetWorldPoint",        &Body::GetWorldPoint},
        {"GetWorldVector",       &Body::GetWorldVector},
        {"GetLocalPoint",        &Body::GetLocalPoint},
        {"GetLocalVector",       &Body::GetLocalVector},
        {"GetLinearVelocityFromWorldPoint",
         &Body::GetLinearVelocityFromWorldPoint},
        {"GetLinearVelocityFromLocalPoint",
         &Body::GetLinearVelocityFromLocalPoint},
        {"GetLinearDamping",     &Body::GetLinearDamping},
        {"SetLinearDamping",     &Body::SetLinearDamping},
        {"GetAngularDamping",    &Body::GetAngularDamping},
        {"SetAngularDamping",    &Body::SetAngularDamping},
        {"GetGravityScale",      &Body::GetGravityScale},
        {"SetGravityScale",      &Body::SetGravityScale},
        {"SetBullet",            &Body::SetBullet},
        {"IsBullet",             &Body::IsBullet},
        {"SetSleepingAllowed",   &Body::SetSleepingAllowed},
        {"IsSleepingAllowed",    &Body::IsSleepingAllowed},
        {"SetAwake",             &Body::SetAwake},
        {"IsAwake",              &Body::IsAwake},
        {"SetActive",            &Body::SetActive},
        {"IsActive",             &Body::IsActive},
        {"SetFixedRotation",     &Body::SetFixedRotation},
        {"IsFixedRotation",      &Body::IsFixedRotation},
        {"GetFixtureList",       &Body::GetFixtureList},
        {"GetJointList",         &Body::GetJointList},
        {"GetContactList",       &Body::GetContactList},
        {"GetNext",              &Body::GetNext},
        {"Dump",                 &Body::Dump},
        {nullptr,                nullptr}};

    void Body::Init(lua_State* L)
    {
        luaR_rawsetinteger(L, "staticBody", b2_staticBody);
        luaR_rawsetinteger(L, "kinematicBody", b2_kinematicBody);
        luaR_rawsetinteger(L, "dynamicBody", b2_dynamicBody);

        luaR_rawsetcfunction(L, "BodyDef", &BodyDef);

        rainbow::lua::reg<Body>(L);
    }

    Body::Body(lua_State* L)
        : body_(static_cast<b2Body*>(lua_touserdata(L, -1))) {}

    int Body::Bind(lua_State* L)
    {
        using rainbow::lua::Sprite;

        // <b2.Body>:Bind(<rainbow.sprite>)
        rainbow::lua::checkargs<Body, Sprite>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        b2Body* body = self->get();
        auto state = static_cast<BodyState*>(body->GetUserData());
        rainbow::lua::replacetable(L, 2);
        state->sprite = rainbow::lua::touserdata<Sprite>(L, 2)->get();
        auto& pos = body->GetPosition();
        state->sprite->set_position(Vec2f(pos.x, pos.y));
        state->sprite->set_rotation(body->GetAngle());
        return 0;
    }


    int Body::CreateFixture(lua_State* L)
    {
        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        b2Fixture* fixture;
        if (lua_gettop(L) == 2)
        {
            // <b2.Body>:CreateFixture(<b2.FixtureDef>)
            rainbow::lua::checkargs<Body, void*>(L);

            const b2FixtureDef& fd = GetFixtureDef(L);
            fixture = self->get()->CreateFixture(&fd);
        }
        else
        {
            // <b2.Body>:CreateFixture(<b2.Shape>, density)
            rainbow::lua::checkargs<Body, IDrawable, lua_Number>(L);

            const float density = lua_tonumber(L, 3);
            lua_settop(L, 2);
            fixture = self->get()->CreateFixture(GetShape(L), density);
        }
        lua_pushlightuserdata(L, fixture);
        return rainbow::lua::alloc<Fixture>(L);
    }

    int Body::DestroyFixture(lua_State* L)
    {
        // <b2.Body>:DestroyFixture(<b2.Fixture>)
        rainbow::lua::checkargs<Body, Fixture>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        auto fixture = rainbow::lua::touserdata<Fixture>(L, 2);
        self->get()->DestroyFixture(fixture->get());
        fixture->reset();
        return 0;
    }

    int Body::SetTransform(lua_State* L)
    {
        // <b2.Body>:SetTransform(x, y, r)
        rainbow::lua::checkargs<Body, lua_Number, lua_Number, lua_Number>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->SetTransform(Vec2(L, 2, 3), lua_tonumber(L, 4));
        return 0;
    }

    int Body::GetPosition(lua_State* L)
    {
        return get1fv(L, [](const b2Body* body) {
            return body->GetPosition();
        });
    }

    int Body::GetAngle(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) { return body->GetAngle(); });
    }

    int Body::GetWorldCenter(lua_State* L)
    {
        return get1fv(L, [](const b2Body* body) {
            return body->GetWorldCenter();
        });
    }

    int Body::GetLocalCenter(lua_State* L)
    {
        return get1fv(L, [](const b2Body* body) {
            return body->GetLocalCenter();
        });
    }

    int Body::SetLinearVelocity(lua_State* L)
    {
        // <b2.Body>:SetLinearVelocity(x, y)
        return set2f(L, [](b2Body* body, float vx, float vy) {
            body->SetLinearVelocity(b2Vec2(vx, vy));
        });
    }

    int Body::GetLinearVelocity(lua_State* L)
    {
        return get1fv(L, [](const b2Body* body) {
            return body->GetLinearVelocity();
        });
    }

    int Body::SetAngularVelocity(lua_State* L)
    {
        // <b2.Body>:SetAngularVelocity(v)
        return set1f(L, [](b2Body* body, float v) {
            body->SetAngularVelocity(v);
        });
    }

    int Body::GetAngularVelocity(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) {
            return body->GetAngularVelocity();
        });
    }

    int Body::ApplyForce(lua_State* L)
    {
        // <b2.Body>:ApplyForce(impulse.x, impulse.y, point.x, point.y, wake)
        rainbow::lua::checkargs<Body,
                                lua_Number,
                                lua_Number,
                                lua_Number,
                                lua_Number,
                                bool>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->ApplyForce(
            Vec2(L, 2, 3), Vec2(L, 4, 5), rainbow::lua::toboolean(L, 6));
        return 0;
    }

    int Body::ApplyForceToCenter(lua_State* L)
    {
        // <b2.Body>:ApplyForceToCenter(impulse.x, impulse.y, wake)
        rainbow::lua::checkargs<Body, lua_Number, lua_Number, bool>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->ApplyForceToCenter(
            Vec2(L, 2, 3), rainbow::lua::toboolean(L, 4));
        return 0;
    }

    int Body::ApplyTorque(lua_State* L)
    {
        // <b2.Body>:ApplyTorque(torque, wake)
        rainbow::lua::checkargs<Body, lua_Number, bool>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->ApplyTorque(
            lua_tonumber(L, 2), rainbow::lua::toboolean(L, 3));
        return 0;
    }

    int Body::ApplyLinearImpulse(lua_State* L)
    {
        // <b2.Body>:ApplyLinearImpulse(impulse.x, impulse.y, point.x, point.y, wake)
        rainbow::lua::checkargs<Body,
                                lua_Number,
                                lua_Number,
                                lua_Number,
                                lua_Number,
                                bool>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->ApplyLinearImpulse(
            Vec2(L, 2, 3), Vec2(L, 4, 5), rainbow::lua::toboolean(L, 6));
        return 0;
    }

    int Body::ApplyAngularImpulse(lua_State* L)
    {
        // <b2.Body>:ApplyAngularImpulse(impulse, wake)
        rainbow::lua::checkargs<Body, lua_Number, bool>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->ApplyAngularImpulse(lua_tonumber(L, 2),
                                         rainbow::lua::toboolean(L, 3));
        return 0;
    }

    int Body::GetMass(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) { return body->GetMass(); });
    }

    int Body::GetInertia(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) { return body->GetInertia(); });
    }

    int Body::GetMassData(lua_State* L)
    {
        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        b2MassData mass;
        self->get()->GetMassData(&mass);
        return MassData(L, mass);
    }

    int Body::SetMassData(lua_State* L)
    {
        rainbow::lua::
            checkargs<Body, lua_Number, lua_Number, lua_Number, lua_Number>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        const b2MassData mass{
            static_cast<float32>(lua_tonumber(L, 2)),
            Vec2(L, 3, 4),
            static_cast<float32>(lua_tonumber(L, 5))};
        self->get()->SetMassData(&mass);
        return 0;
    }

    int Body::ResetMassData(lua_State* L)
    {
        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->ResetMassData();
        return 0;
    }

    int Body::GetWorldPoint(lua_State* L)
    {
        rainbow::lua::checkargs<Body, lua_Number, lua_Number>(L);

        return get1fv(L,
                      [](const b2Body* body, const b2Vec2& p) {
                          return body->GetWorldPoint(p);
                      },
                      Vec2(L, 2, 3));
    }

    int Body::GetWorldVector(lua_State* L)
    {
        rainbow::lua::checkargs<Body, lua_Number, lua_Number>(L);

        return get1fv(L,
                      [](const b2Body* body, const b2Vec2& v) {
                          return body->GetWorldVector(v);
                      },
                      Vec2(L, 2, 3));
    }

    int Body::GetLocalPoint(lua_State* L)
    {
        rainbow::lua::checkargs<Body, lua_Number, lua_Number>(L);

        return get1fv(L,
                      [](const b2Body* body, const b2Vec2& p) {
                          return body->GetLocalPoint(p);
                      },
                      Vec2(L, 2, 3));
    }

    int Body::GetLocalVector(lua_State* L)
    {
        rainbow::lua::checkargs<Body, lua_Number, lua_Number>(L);

        return get1fv(L,
                      [](const b2Body* body, const b2Vec2& v) {
                          return body->GetLocalVector(v);
                      },
                      Vec2(L, 2, 3));
    }

    int Body::GetLinearVelocityFromWorldPoint(lua_State* L)
    {
        rainbow::lua::checkargs<Body, lua_Number, lua_Number>(L);

        return get1fv(L,
                      [](const b2Body* body, const b2Vec2& p) {
                          return body->GetLinearVelocityFromWorldPoint(p);
                      },
                      Vec2(L, 2, 3));
    }

    int Body::GetLinearVelocityFromLocalPoint(lua_State* L)
    {
        rainbow::lua::checkargs<Body, lua_Number, lua_Number>(L);

        return get1fv(L,
                      [](const b2Body* body, const b2Vec2& p) {
                          return body->GetLinearVelocityFromLocalPoint(p);
                      },
                      Vec2(L, 2, 3));
    }

    int Body::GetLinearDamping(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) {
            return body->GetLinearDamping();
        });
    }

    int Body::SetLinearDamping(lua_State* L)
    {
        // <b2.Body>:SetLinearDamping(damping)
        rainbow::lua::checkargs<Body, lua_Number>(L);

        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->SetLinearDamping(lua_tonumber(L, 2));
        return 0;
    }

    int Body::GetAngularDamping(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) {
            return body->GetAngularDamping();
        });
    }

    int Body::SetAngularDamping(lua_State* L)
    {
        // <b2.Body>:SetAngularDamping(damping)
        return set1f(L, [](b2Body* body, float damping) {
            body->SetAngularDamping(damping);
        });
    }

    int Body::GetGravityScale(lua_State* L)
    {
        return get1f(L, [](const b2Body* body) {
            return body->GetGravityScale();
        });
    }

    int Body::SetGravityScale(lua_State* L)
    {
        // <b2.Body>:SetGravityScale(scale)
        return set1f(L, [](b2Body* body, float scale) {
            body->SetGravityScale(scale);
        });
    }

    int Body::SetBullet(lua_State* L)
    {
        // <b2.Body>:SetBullet(bool)
        return set1b(L, [](b2Body* body, bool bullet) {
            body->SetBullet(bullet);
        });
    }

    int Body::IsBullet(lua_State* L)
    {
        return get1b(L, [](const b2Body* body) { return body->IsBullet(); });
    }

    int Body::SetSleepingAllowed(lua_State* L)
    {
        // <b2.Body>:SetSleepingAllowed(bool)
        return set1b(L, [](b2Body* body, bool allowed) {
            body->SetSleepingAllowed(allowed);
        });
    }

    int Body::IsSleepingAllowed(lua_State* L)
    {
        return get1b(L, [](const b2Body* body) {
            return body->IsSleepingAllowed();
        });
    }

    int Body::SetAwake(lua_State* L)
    {
        // <b2.Body>:SetAwake(bool)
        return set1b(L, [](b2Body* body, bool awake) {
            body->SetAwake(awake);
        });
    }

    int Body::IsAwake(lua_State* L)
    {
        return get1b(L, [](const b2Body* body) {
            return body->IsAwake();
        });
    }

    int Body::SetActive(lua_State* L)
    {
        // <b2.Body>:SetActive(bool)
        return set1b(L, [](b2Body* body, bool active) {
            body->SetActive(active);
        });
    }

    int Body::IsActive(lua_State* L)
    {
        return get1b(L, [](const b2Body* body) {
            return body->IsActive();
        });
    }

    int Body::SetFixedRotation(lua_State* L)
    {
        // <b2.Body>:SetFixedRotation(bool)
        return set1b(L, [](b2Body* body, bool fixed) {
            body->SetFixedRotation(fixed);
        });
    }

    int Body::IsFixedRotation(lua_State* L)
    {
        return get1b(L, [](const b2Body* body) {
            return body->IsFixedRotation();
        });
    }

    int Body::GetFixtureList(lua_State* L)
    {
        return get1ud<Fixture>(L, [](b2Body* body) {
            return body->GetFixtureList();
        });
    }

    int Body::GetJointList(lua_State*) { return -1; }

    int Body::GetContactList(lua_State*) { return -1; }

    int Body::GetNext(lua_State* L)
    {
        return get1ud<Body>(L, [](b2Body* body) { return body->GetNext(); });
    }

    int Body::Dump(lua_State* L)
    {
        Body* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->get()->Dump();
        return 0;
    }
} NS_B2_LUA_END
