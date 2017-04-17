// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Dynamics/Contacts/Contact.h"

#include <Box2D/Dynamics/Contacts/b2Contact.h>

#include "ThirdParty/Box2D/Lua/Dynamics/Fixture.h"

using b2::lua::Contact;

Contact::Contact(lua_State*) : contact_(nullptr)
{
}

auto Contact::GetManifold(lua_State*) -> int
{
    return -1;
}

auto Contact::GetWorldManifold(lua_State*) -> int
{
    return -1;
}

auto Contact::IsTouching(lua_State* L) -> int
{
    return get1b(L, [](const b2Contact* contact) {
        return contact->IsTouching();
    });
}

auto Contact::SetEnabled(lua_State* L) -> int
{
    return set1b(L, [](b2Contact* contact, bool enabled) {
        contact->SetEnabled(enabled);
    });
}

auto Contact::IsEnabled(lua_State* L) -> int
{
    return get1b(L, [](const b2Contact* contact) {
        return contact->IsEnabled();
    });
}

auto Contact::GetNext(lua_State* L) -> int
{
    return with_self(L, [](Contact* self, lua_State* L) {
        self->reset(self->get()->GetNext());
        rainbow::lua::push(L, self->get() != nullptr);
        return 1;
    });
}

auto Contact::GetFixtureA(lua_State* L) -> int
{
    return get1ud<Fixture>(L, [](b2Contact* contact) {
        return contact->GetFixtureA();
    });
}

auto Contact::GetChildIndexA(lua_State* L) -> int
{
    return get1i(L, [](const b2Contact* contact) {
        return contact->GetChildIndexA();
    });
}

auto Contact::GetFixtureB(lua_State* L) -> int
{
    return get1ud<Fixture>(L, [](b2Contact* contact) {
        return contact->GetFixtureB();
    });
}

auto Contact::GetChildIndexB(lua_State* L) -> int
{
    return get1i(L, [](const b2Contact* contact) {
        return contact->GetChildIndexB();
    });
}

auto Contact::SetFriction(lua_State* L) -> int
{
    return set1f(L, [](b2Contact* contact, float friction) {
        contact->SetFriction(friction);
    });
}

auto Contact::GetFriction(lua_State* L) -> int
{
    return get1f(L, [](const b2Contact* contact) {
        return contact->GetFriction();
    });
}

auto Contact::ResetFriction(lua_State* L) -> int
{
    return with_self(L, [](Contact* self, lua_State*) {
        self->get()->ResetFriction();
        return 0;
    });
}

auto Contact::SetRestitution(lua_State* L) -> int
{
    return set1f(L, [](b2Contact* contact, float r) {
        contact->SetRestitution(r);
    });
}

auto Contact::GetRestitution(lua_State* L) -> int
{
    return get1f(L, [](const b2Contact* contact) {
        return contact->GetRestitution();
    });
}

auto Contact::ResetRestitution(lua_State* L) -> int
{
    return with_self(L, [](Contact* self, lua_State*) {
        self->get()->ResetRestitution();
        return 0;
    });
}

auto Contact::SetTangentSpeed(lua_State* L) -> int
{
    return set1f(L, [](b2Contact* contact, float speed) {
        contact->SetTangentSpeed(speed);
    });
}

auto Contact::GetTangentSpeed(lua_State* L) -> int
{
    return get1f(L, [](const b2Contact* contact) {
        return contact->GetTangentSpeed();
    });
}

LUA_REG_OBJECT(Contact, "b2Contact") {
    LUA_REG_OBJECT_FUNC(Contact, GetManifold),
    LUA_REG_OBJECT_FUNC(Contact, GetWorldManifold),
    LUA_REG_OBJECT_FUNC(Contact, IsTouching),
    LUA_REG_OBJECT_FUNC(Contact, SetEnabled),
    LUA_REG_OBJECT_FUNC(Contact, IsEnabled),
    LUA_REG_OBJECT_FUNC(Contact, GetNext),
    LUA_REG_OBJECT_FUNC(Contact, GetFixtureA),
    LUA_REG_OBJECT_FUNC(Contact, GetChildIndexA),
    LUA_REG_OBJECT_FUNC(Contact, GetFixtureB),
    LUA_REG_OBJECT_FUNC(Contact, GetChildIndexB),
    LUA_REG_OBJECT_FUNC(Contact, SetFriction),
    LUA_REG_OBJECT_FUNC(Contact, GetFriction),
    LUA_REG_OBJECT_FUNC(Contact, ResetFriction),
    LUA_REG_OBJECT_FUNC(Contact, SetRestitution),
    LUA_REG_OBJECT_FUNC(Contact, GetRestitution),
    LUA_REG_OBJECT_FUNC(Contact, ResetRestitution),
    LUA_REG_OBJECT_FUNC(Contact, SetTangentSpeed),
    LUA_REG_OBJECT_FUNC(Contact, GetTangentSpeed),
    LUA_REG_OBJECT_END()
};
