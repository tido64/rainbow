// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_B2FIXTURE_H_
#define THIRDPARTY_BOX2D_LUA_B2FIXTURE_H_

NS_B2_LUA_BEGIN
{
	int Filter(lua_State *L)
	{
		lua_createtable(L, 0, 3);
		luaR_rawsetinteger(L, "categoryBits", 0x0001);
		luaR_rawsetinteger(L, "maskBits", 0xFFFF);
		luaR_rawsetinteger(L, "groupIndex", 0);
		return 1;
	}

	void parse(lua_State *L, b2Filter &filter)
	{
		filter.categoryBits = luaR_getinteger(L, "categoryBits");
		filter.maskBits = luaR_getinteger(L, "maskBits");
		filter.groupIndex = luaR_getinteger(L, "groupIndex");
	}

	int FixtureDef(lua_State *L)
	{
		lua_createtable(L, 0, 6);

		luaR_rawsetnil(L, "shape");
		luaR_rawsetnumber(L, "friction", 0.2f);
		luaR_rawsetnumber(L, "restitution", 0.0f);
		luaR_rawsetnumber(L, "density", 0.0f);
		luaR_rawsetboolean(L, "isSensor", false);

		lua_pushliteral(L, "filter");
		Filter(L);
		lua_rawset(L, -3);

		return 1;
	}

	void parse(lua_State *L, b2FixtureDef &def)
	{
		luaR_getfield(L, "shape");
		def.shape = parse_Shape(L);
		lua_pop(L, 1);

		def.friction = luaR_getnumber(L, "friction");
		def.restitution = luaR_getnumber(L, "restitution");
		def.density = luaR_getnumber(L, "density");
		def.isSensor = luaR_getboolean(L, "isSensor");

		luaR_getfield(L, "filter");
		parse(L, def.filter);
		lua_pop(L, 1);
	}

	class Fixture : public Bind<Fixture>
	{
		friend Bind;

	public:
		Fixture(lua_State *);

		inline void set(b2Fixture *fixture);

	private:
		static int get_body(lua_State *);

		b2Fixture *fixture_;
	};

	Fixture::Fixture(lua_State *) : fixture_(nullptr) {}

	void Fixture::set(b2Fixture *fixture)
	{
		fixture_ = fixture;
	}

	int Fixture::get_body(lua_State *L)
	{
		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		lua_rawgeti(L, LUA_REGISTRYINDEX, g_body_list);
		lua_pushlightuserdata(L, self->fixture_->GetBody());
		lua_gettable(L, -2);
		lua_remove(L, -2);
		R_ASSERT(lua_istable(L, -1), "Body wasn't properly registered");
		return 1;
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::Lua::Fixture;

	template<>
	const char Fixture::Bind::class_name[] = "Fixture";

	template<>
	const bool Fixture::Bind::is_constructible = false;

	template<>
	const luaL_Reg Fixture::Bind::functions[] = {
		{ "GetBody",  &Fixture::get_body },
		{ nullptr,    nullptr }
	};
} NS_RAINBOW_LUA_END

#endif
