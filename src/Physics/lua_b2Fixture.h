// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PHYSICS_LUA_B2FIXTURE_H_
#define PHYSICS_LUA_B2FIXTURE_H_

NS_B2_LUA_BEGIN
{
	int Filter(lua_State *L)
	{
		lua_createtable(L, 0, 3);

		luaR_rawsetfield(L, lua_pushinteger, 0x0001, "categoryBits");
		luaR_rawsetfield(L, lua_pushinteger, 0xFFFF, "maskBits");
		luaR_rawsetfield(L, lua_pushinteger, 0, "groupIndex");

		return 1;
	}

	void parse_Filter(lua_State *L, b2Filter &filter)
	{
		const char type[] = "Filter";
		static_cast<void>(type);

		luaR_rawgetfield(L, "categoryBits", type);
		filter.categoryBits = Rainbow::Lua::tointeger(L, -1);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "maskBits", type);
		filter.maskBits = Rainbow::Lua::tointeger(L, -1);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "groupIndex", type);
		filter.groupIndex = Rainbow::Lua::tointeger(L, -1);
		lua_pop(L, 1);
	}

	int FixtureDef(lua_State *L)
	{
		lua_createtable(L, 0, 6);

		luaR_rawsetnilfield(L, "shape");
		luaR_rawsetfield(L, lua_pushnumber, 0.2f, "friction");
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "restitution");
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "density");
		luaR_rawsetfield(L, lua_pushboolean, false, "isSensor");

		lua_pushliteral(L, "filter");
		Filter(L);
		lua_rawset(L, -3);

		return 1;
	}

	void parse_FixtureDef(lua_State *L, b2FixtureDef &def)
	{
		const char type[] = "FixtureDef";
		static_cast<void>(type);

		luaR_rawgetfield(L, "shape", type);
		def.shape = parse_Shape(L);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "friction", type);
		def.friction = Rainbow::Lua::tonumber(L, -1);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "restitution", type);
		def.restitution = Rainbow::Lua::tonumber(L, -1);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "density", type);
		def.density = Rainbow::Lua::tonumber(L, -1);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "isSensor", type);
		def.isSensor = lua_toboolean(L, -1);
		lua_pop(L, 1);

		luaR_rawgetfield(L, "filter", type);
		parse_Filter(L, def.filter);
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

	Fixture::Fixture(lua_State *) : fixture_(nullptr) { }

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
