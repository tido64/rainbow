namespace b2
{
	namespace Lua
	{
		namespace
		{
			/// Cached fixture object for Contact::GetFixtureA() and
			/// Contact::GetFixtureB().
			int g_fixture = LUA_REFNIL;
		}

		class Contact :
			public Rainbow::Lua::Bind<Contact , b2Contact, Rainbow::Lua::kBindTypeWeak>
		{
			friend class Rainbow::Lua::Bind<Contact , b2Contact, Rainbow::Lua::kBindTypeWeak>;

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
	}
}

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<b2::Lua::Contact, b2Contact, kBindTypeWeak> b2LuaContact;

		template<>
		const char b2LuaContact::class_name[] = "Contact";

		template<>
		const Method<b2::Lua::Contact> b2LuaContact::methods[] = {
			{ "IsTouching",   &b2::Lua::Contact::is_touching },
			{ "SetEnabled",   &b2::Lua::Contact::set_enabled },
			{ "IsEnabled",    &b2::Lua::Contact::is_enabled },
			{ "GetNext",      &b2::Lua::Contact::get_next },
			{ "GetFixtureA",  &b2::Lua::Contact::get_fixture_a },
			{ "GetFixtureB",  &b2::Lua::Contact::get_fixture_b },
			{ nullptr,        nullptr }
		};
	}
}
