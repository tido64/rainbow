// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Debug.h"
#include "Graphics/Shaders/Diffuse.h"
#include "Lua/LuaBind.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Shaders.h"

namespace
{
	class Diffuse : public Rainbow::Lua::Shader,
	                public Rainbow::Lua::Bind<Diffuse>
	{
		friend Bind;

	public:
		Diffuse(lua_State *);

		const Rainbow::Shaders::Diffuse* get() const;

		virtual int id() const override;

	private:
		static int set_cutoff(lua_State *);
		static int set_radius(lua_State *);
		static int set_position(lua_State *);

		Rainbow::Shaders::Diffuse lighting;
	};

	int diffuse(lua_State *L)
	{
		return Rainbow::Lua::alloc<Diffuse>(L);
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(Shaders)
{
	void init(lua_State *L)
	{
		// Initialise "rainbow.shaders" namespace
		lua_pushliteral(L, "shaders");
		lua_createtable(L, 0, 1);
		luaR_rawsetcclosurefield(L, &diffuse, "diffuse");
		lua_rawset(L, -3);

		reg<Diffuse>(L);
	}
} NS_RAINBOW_LUA_MODULE_END(Shaders)

template<>
const char Diffuse::Bind::class_name[] = "shaders.diffuse";

template<>
const bool Diffuse::Bind::is_constructible = false;

template<>
const luaL_Reg Diffuse::Bind::functions[] = {
	{ "set_cutoff",    &Diffuse::set_cutoff },
	{ "set_radius",    &Diffuse::set_radius },
	{ "set_position",  &Diffuse::set_position },
	{ nullptr, nullptr }
};

Diffuse::Diffuse(lua_State *L) : lighting(lua_toboolean(L, 1)) { }

const Rainbow::Shaders::Diffuse* Diffuse::get() const
{
	return &this->lighting;
}

int Diffuse::id() const
{
	return this->lighting.id();
}

int Diffuse::set_cutoff(lua_State *L)
{
	LUA_ASSERT(lua_isnumber(L, 2), "<diffuse>:set_cutoff(cutoff)");

	Diffuse *self = Bind::self(L);
	if (!self)
		return 0;

	self->lighting.set_cutoff(lua_tonumber(L, 2));
	return 0;
}

int Diffuse::set_radius(lua_State *L)
{
	LUA_ASSERT(lua_isnumber(L, 2), "<diffuse>:set_radius(radius)");

	Diffuse *self = Bind::self(L);
	if (!self)
		return 0;

	self->lighting.set_radius(lua_tonumber(L, 2));
	return 0;
}

int Diffuse::set_position(lua_State *L)
{
	LUA_ASSERT(lua_isnumber(L, 2) &&
	           lua_isnumber(L, 3) &&
	           (lua_isnumber(L, 4) || lua_isnone(L, 4)),
	           "<diffuse>:set_position(x, y, z = 100.0)");

	Diffuse *self = Bind::self(L);
	if (!self)
		return 0;

	self->lighting.set_position(lua_tonumber(L, 2),
	                            lua_tonumber(L, 3),
	                            luaR_optnumber(L, 4, 100.0));
	return 0;
}
