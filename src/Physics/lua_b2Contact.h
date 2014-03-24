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

	class Contact
	    : public Rainbow::Lua::Bind<Contact, b2Contact, Rainbow::Lua::kBindTypeWeak>
	{
		friend Bind;

	public:
		inline Contact(lua_State *);
		inline void set(b2Contact *contact);

	private:
		int is_touching(lua_State *);
		int set_enabled(lua_State *);
		int is_enabled(lua_State *);

		int get_next(lua_State *);
		int get_fixture_a(lua_State *);
		int get_fixture_b(lua_State *);

		int set_friction(lua_State *);
		int get_friction(lua_State *);
		int reset_friction(lua_State *);

		int set_restitution(lua_State *);
		int get_restitution(lua_State *);
		int reset_restitution(lua_State *);

		int set_tangent_speed(lua_State *);
		int get_tangent_speed(lua_State *);

		int get_fixture(lua_State *, b2Fixture *);
	};

	Contact::Contact(lua_State *) { }

	void Contact::set(b2Contact *contact)
	{
		this->ptr = contact;
	}

	int Contact::is_touching(lua_State *L)
	{
		lua_pushboolean(L, this->ptr->IsTouching());
		return 1;
	}

	int Contact::set_enabled(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) == 1, "<b2.contact>:set_enabled(bool)");

		this->ptr->SetEnabled(lua_toboolean(L, 1));
		return 0;
	}

	int Contact::is_enabled(lua_State *L)
	{
		lua_pushboolean(L, this->ptr->IsEnabled());
		return 1;
	}

	int Contact::get_next(lua_State *L)
	{
		this->ptr = this->ptr->GetNext();
		lua_pushboolean(L, !!this->ptr);
		return 1;
	}

	int Contact::get_fixture_a(lua_State *L)
	{
		if (!this->ptr)
			return 0;
		return this->get_fixture(L, this->ptr->GetFixtureA());
	}

	int Contact::get_fixture_b(lua_State *L)
	{
		if (!this->ptr)
			return 0;
		return this->get_fixture(L, this->ptr->GetFixtureB());
	}

	int Contact::get_fixture(lua_State *L, b2Fixture *fixture)
	{
		if (g_fixture == LUA_REFNIL)
		{
			Rainbow::Lua::alloc<Fixture>(L);
			g_fixture = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		lua_rawgeti(L, LUA_REGISTRYINDEX, g_fixture);
		lua_rawgeti(L, -1, 0);
		Fixture *f = static_cast<Fixture*>(luaR_touserdata(L, -1, Fixture::class_name));
		f->set(fixture);
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
	const Method<Contact> Contact::Bind::methods[] = {
		{ "IsTouching",   &Contact::is_touching },
		{ "SetEnabled",   &Contact::set_enabled },
		{ "IsEnabled",    &Contact::is_enabled },
		{ "GetNext",      &Contact::get_next },
		{ "GetFixtureA",  &Contact::get_fixture_a },
		{ "GetFixtureB",  &Contact::get_fixture_b },
		{ nullptr,        nullptr }
	};
} NS_RAINBOW_LUA_END
