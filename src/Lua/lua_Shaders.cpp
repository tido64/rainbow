// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Shaders.h"

#include "Graphics/Shaders/Diffuse.h"
#include "Lua/LuaBind.h"

namespace
{
	class Diffuse final : public rainbow::lua::Shader,
	                      public rainbow::lua::Bind<Diffuse>
	{
	public:
		explicit Diffuse(lua_State* L) : lighting_(lua_toboolean(L, 1)) {}

		const rainbow::shaders::Diffuse* get() const { return &lighting_; }

		int id() const override { return lighting_.id(); }

	private:
		static int set_cutoff(lua_State*);
		static int set_radius(lua_State*);
		static int set_position(lua_State*);

		rainbow::shaders::Diffuse lighting_;

		friend Bind;
	};

	int diffuse(lua_State* L) { return rainbow::lua::alloc<Diffuse>(L); }
}

NS_RAINBOW_LUA_MODULE_BEGIN(shaders)
{
	void init(lua_State* L)
	{
		// Initialise "rainbow.shaders" namespace
		lua_pushliteral(L, "shaders");
		lua_createtable(L, 0, 1);
		luaR_rawsetcfunction(L, "diffuse", &diffuse);
		lua_rawset(L, -3);

		reg<Diffuse>(L);
	}
} NS_RAINBOW_LUA_MODULE_END(shaders)

template <>
const char Diffuse::Bind::class_name[] = "shaders.diffuse";

template <>
const bool Diffuse::Bind::is_constructible = false;

template <>
const luaL_Reg Diffuse::Bind::functions[]{
    {"set_cutoff",    &Diffuse::set_cutoff},
    {"set_radius",    &Diffuse::set_radius},
    {"set_position",  &Diffuse::set_position},
    {nullptr,         nullptr}};

int Diffuse::set_cutoff(lua_State* L)
{
	// <diffuse>:set_cutoff(cutoff)
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	Diffuse* self = Bind::self(L);
	if (!self)
		return 0;

	self->lighting_.set_cutoff(lua_tonumber(L, 2));
	return 0;
}

int Diffuse::set_radius(lua_State* L)
{
	// <diffuse>:set_radius(radius)
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);

	Diffuse* self = Bind::self(L);
	if (!self)
		return 0;

	self->lighting_.set_radius(lua_tonumber(L, 2));
	return 0;
}

int Diffuse::set_position(lua_State* L)
{
	// <diffuse>:set_position(x, y, z = 100.0)
	rainbow::lua::Argument<lua_Number>::is_required(L, 2);
	rainbow::lua::Argument<lua_Number>::is_required(L, 3);
	rainbow::lua::Argument<lua_Number>::is_optional(L, 4);

	Diffuse* self = Bind::self(L);
	if (!self)
		return 0;

	self->lighting_.set_position(lua_tonumber(L, 2),
	                             lua_tonumber(L, 3),
	                             rainbow::lua::optnumber(L, 4, 100.0));
	return 0;
}
