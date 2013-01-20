namespace b2
{
	namespace Lua
	{
		namespace
		{
			/// Cached fixture object for Contact::GetFixtureA() and
			/// Contact::GetFixtureB().
			int g_fixture = -1;
		}

		class Contact
		{
		public:
			static const char class_name[];
			static const Rainbow::Lua::Method<Contact> methods[];

			b2Contact *contact;

			Contact(lua_State *);

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

		private:
			int get_fixture(lua_State *, b2Fixture *);
		};

		const char Contact::class_name[] = "Contact";
		const Rainbow::Lua::Method<Contact> Contact::methods[] = {
			{ "IsTouching",   &Contact::is_touching },
			{ "SetEnabled",   &Contact::set_enabled },
			{ "IsEnabled",    &Contact::is_enabled },
			{ "GetNext",      &Contact::get_next },
			{ "GetFixtureA",  &Contact::get_fixture_a },
			{ "GetFixtureB",  &Contact::get_fixture_b },
			{ nullptr,        nullptr }
		};

		Contact::Contact(lua_State *) : contact(nullptr) { }

		int Contact::is_touching(lua_State *L)
		{
			lua_pushboolean(L, this->contact->IsTouching());
			return 1;
		}

		int Contact::set_enabled(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<b2.contact>:set_enabled(bool)");

			this->contact->SetEnabled(lua_toboolean(L, 1));
			return 0;
		}

		int Contact::is_enabled(lua_State *L)
		{
			lua_pushboolean(L, this->contact->IsEnabled());
			return 1;
		}

		int Contact::get_next(lua_State *L)
		{
			this->contact = this->contact->GetNext();
			lua_pushboolean(L, !!this->contact);
			return 1;
		}

		int Contact::get_fixture_a(lua_State *L)
		{
			if (!this->contact)
				return 0;
			return this->get_fixture(L, this->contact->GetFixtureA());
		}

		int Contact::get_fixture_b(lua_State *L)
		{
			if (!this->contact)
				return 0;
			return this->get_fixture(L, this->contact->GetFixtureB());
		}

		int Contact::get_fixture(lua_State *L, b2Fixture *fixture)
		{
			if (g_fixture < 0)
			{
				Rainbow::Lua::alloc<Fixture>(L);
				g_fixture = luaL_ref(L, LUA_REGISTRYINDEX);
			}
			lua_rawgeti(L, LUA_REGISTRYINDEX, g_fixture);
			lua_rawgeti(L, -1, 0);
			Fixture **fix = static_cast<Fixture**>(lua_touserdata(L, -1));
			(*fix)->fixture = fixture;
			lua_pop(L, 1);
			return 1;
		}
	}
}
