// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PHYSICS_LUA_B2CONTACT_H_
#define PHYSICS_LUA_B2CONTACT_H_

NS_B2_LUA_BEGIN
{
	namespace
	{
		/// Cached fixture object for Contact::GetFixtureA() and
		/// Contact::GetFixtureB().
		int g_fixture = LUA_REFNIL;
	}

	class Contact : public Bind<Contact>
	{
		friend Bind;

	public:
		inline Contact(lua_State *);
		inline void set(b2Contact *contact);

	private:
		static int is_touching(lua_State *);
		static int set_enabled(lua_State *);
		static int is_enabled(lua_State *);

		static int get_next(lua_State *);
		static int get_fixture_a(lua_State *);
		static int get_fixture_b(lua_State *);

		static int set_friction(lua_State *);
		static int get_friction(lua_State *);
		static int reset_friction(lua_State *);

		static int set_restitution(lua_State *);
		static int get_restitution(lua_State *);
		static int reset_restitution(lua_State *);

		static int set_tangent_speed(lua_State *);
		static int get_tangent_speed(lua_State *);

		static int get_fixture(lua_State *, b2Fixture *);

		b2Contact *contact_;
	};

	Contact::Contact(lua_State *) : contact_(nullptr) { }

	void Contact::set(b2Contact *contact)
	{
		contact_ = contact;
	}

	int Contact::is_touching(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->contact_->IsTouching());
		return 1;
	}

	int Contact::set_enabled(lua_State *L)
	{
		// <b2.contact>:set_enabled(bool)
		Rainbow::Lua::Argument<bool>::is_required(L, 2);

		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		self->contact_->SetEnabled(lua_toboolean(L, 2));
		return 0;
	}

	int Contact::is_enabled(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->contact_->IsEnabled());
		return 1;
	}

	int Contact::get_next(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		self->contact_ = self->contact_->GetNext();
		lua_pushboolean(L, !!self->contact_);
		return 1;
	}

	int Contact::get_fixture_a(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self || !self->contact_)
			return 0;

		return get_fixture(L, self->contact_->GetFixtureA());
	}

	int Contact::get_fixture_b(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self || !self->contact_)
			return 0;

		return get_fixture(L, self->contact_->GetFixtureB());
	}

	int Contact::get_fixture(lua_State *L, b2Fixture *fixture)
	{
		if (g_fixture == LUA_REFNIL)
		{
			Rainbow::Lua::alloc<Fixture>(L);
			g_fixture = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		lua_rawgeti(L, LUA_REGISTRYINDEX, g_fixture);
		Rainbow::Lua::touserdata<Fixture>(L, -1)->set(fixture);
		lua_pop(L, 1);
		return 1;
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::Lua::Contact;

	template<>
	const char Contact::Bind::class_name[] = "Contact";

	template<>
	const bool Contact::Bind::is_constructible = false;

	template<>
	const luaL_Reg Contact::Bind::functions[] = {
		{ "IsTouching",   &Contact::is_touching },
		{ "SetEnabled",   &Contact::set_enabled },
		{ "IsEnabled",    &Contact::is_enabled },
		{ "GetNext",      &Contact::get_next },
		{ "GetFixtureA",  &Contact::get_fixture_a },
		{ "GetFixtureB",  &Contact::get_fixture_b },
		{ nullptr,        nullptr }
	};
} NS_RAINBOW_LUA_END

#endif
