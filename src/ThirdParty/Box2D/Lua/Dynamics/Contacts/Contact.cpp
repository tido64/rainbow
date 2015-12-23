// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Dynamics/Contacts/Contact.h"

#include <Box2D/Dynamics/Contacts/b2Contact.h>

#include "ThirdParty/Box2D/Lua/Dynamics/Fixture.h"

NS_B2_LUA_BEGIN
{
	Contact::Contact(lua_State*) : contact_(nullptr) {}

	int Contact::GetManifold(lua_State*) { return -1; }

	int Contact::GetWorldManifold(lua_State*) { return -1; }

	int Contact::IsTouching(lua_State* L)
	{
		return get1b(L, [](const b2Contact* contact) {
			return contact->IsTouching();
		});
	}

	int Contact::SetEnabled(lua_State* L)
	{
		return set1b(L, [](b2Contact* contact, bool enabled) {
			contact->SetEnabled(enabled);
		});
	}

	int Contact::IsEnabled(lua_State* L)
	{
		return get1b(L, [](const b2Contact* contact) {
			return contact->IsEnabled();
		});
	}

	int Contact::GetNext(lua_State* L)
	{
		Contact* self = Bind::self(L);
		if (!self)
			return 0;

		self->reset(self->get()->GetNext());
		lua_pushboolean(L, !!self->get());
		return 1;
	}

	int Contact::GetFixtureA(lua_State* L)
	{
		return get1ud<Fixture>(L, [](b2Contact* contact) {
			return contact->GetFixtureA();
		});
	}

	int Contact::GetChildIndexA(lua_State* L)
	{
		return get1i(L, [](const b2Contact* contact) {
			return contact->GetChildIndexA();
		});
	}

	int Contact::GetFixtureB(lua_State* L)
	{
		return get1ud<Fixture>(L, [](b2Contact* contact) {
			return contact->GetFixtureB();
		});
	}

	int Contact::GetChildIndexB(lua_State* L)
	{
		return get1i(L, [](const b2Contact* contact) {
			return contact->GetChildIndexB();
		});
	}

	int Contact::SetFriction(lua_State* L)
	{
		return set1f(L, [](b2Contact* contact, float friction) {
			contact->SetFriction(friction);
		});
	}

	int Contact::GetFriction(lua_State* L)
	{
		return get1f(L, [](const b2Contact* contact) {
			return contact->GetFriction();
		});
	}

	int Contact::ResetFriction(lua_State* L)
	{
		Contact* self = Bind::self(L);
		if (!self)
			return 0;

		self->get()->ResetFriction();
		return 0;
	}

	int Contact::SetRestitution(lua_State* L)
	{
		return set1f(L, [](b2Contact* contact, float r) {
			contact->SetRestitution(r);
		});
	}

	int Contact::GetRestitution(lua_State* L)
	{
		return get1f(L, [](const b2Contact* contact) {
			return contact->GetRestitution();
		});
	}

	int Contact::ResetRestitution(lua_State* L)
	{
		Contact* self = Bind::self(L);
		if (!self)
			return 0;

		self->get()->ResetRestitution();
		return 0;
	}

	int Contact::SetTangentSpeed(lua_State* L)
	{
		return set1f(L, [](b2Contact* contact, float speed) {
			contact->SetTangentSpeed(speed);
		});
	}

	int Contact::GetTangentSpeed(lua_State* L)
	{
		return get1f(L, [](const b2Contact* contact) {
			return contact->GetTangentSpeed();
		});
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::lua::Contact;

	template <>
	const char Contact::Bind::class_name[] = "b2Contact";

	template <>
	const bool Contact::Bind::is_constructible = false;

	template <>
	const luaL_Reg Contact::Bind::functions[]{
	    {"GetManifold",       &Contact::GetManifold},
	    {"GetWorldManifold",  &Contact::GetWorldManifold},
	    {"IsTouching",        &Contact::IsTouching},
	    {"SetEnabled",        &Contact::SetEnabled},
	    {"IsEnabled",         &Contact::IsEnabled},
	    {"GetNext",           &Contact::GetNext},
	    {"GetFixtureA",       &Contact::GetFixtureA},
	    {"GetChildIndexA",    &Contact::GetChildIndexA},
	    {"GetFixtureB",       &Contact::GetFixtureB},
	    {"GetChildIndexB",    &Contact::GetChildIndexB},
	    {"SetFriction",       &Contact::SetFriction},
	    {"GetFriction",       &Contact::GetFriction},
	    {"ResetFriction",     &Contact::ResetFriction},
	    {"SetRestitution",    &Contact::SetRestitution},
	    {"GetRestitution",    &Contact::GetRestitution},
	    {"ResetRestitution",  &Contact::ResetRestitution},
	    {"SetTangentSpeed",   &Contact::SetTangentSpeed},
	    {"GetTangentSpeed",   &Contact::GetTangentSpeed},
	    {nullptr,             nullptr}
	};
} NS_RAINBOW_LUA_END
