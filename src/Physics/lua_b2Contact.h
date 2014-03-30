// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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

		b2Contact *contact;
	};

	Contact::Contact(lua_State *) : contact(nullptr) { }

	void Contact::set(b2Contact *contact)
	{
		this->contact = contact;
	}

	int Contact::is_touching(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->contact->IsTouching());
		return 1;
	}

	int Contact::set_enabled(lua_State *L)
	{
		LUA_ASSERT(lua_isboolean(L, 2), "<b2.contact>:set_enabled(bool)");

		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		self->contact->SetEnabled(lua_toboolean(L, 2));
		return 0;
	}

	int Contact::is_enabled(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->contact->IsEnabled());
		return 1;
	}

	int Contact::get_next(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self)
			return 0;

		self->contact = self->contact->GetNext();
		lua_pushboolean(L, !!self->contact);
		return 1;
	}

	int Contact::get_fixture_a(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self || !self->contact)
			return 0;

		return get_fixture(L, self->contact->GetFixtureA());
	}

	int Contact::get_fixture_b(lua_State *L)
	{
		Contact *self = Bind::self(L);
		if (!self || !self->contact)
			return 0;

		return get_fixture(L, self->contact->GetFixtureB());
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
