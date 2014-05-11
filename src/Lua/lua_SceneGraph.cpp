// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_SceneGraph.h"

#include "Graphics/Drawable.h"
#include "Graphics/SceneGraph.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

namespace
{
	template<class T, Rainbow::Lua::SceneGraph::CastingMethod C>
	struct luaR_cast;

	template<class T>
	struct luaR_cast<T, Rainbow::Lua::SceneGraph::kCastingSafe>
	{
		static T* from(lua_State *L, const int n)
		{
			return static_cast<T*>(luaL_checkudata(L, n, T::class_name));
		}
	};

	template<class T>
	struct luaR_cast<T, Rainbow::Lua::SceneGraph::kCastingUnsafe>
	{
		static T* from(lua_State *L, const int n)
		{
			return static_cast<T*>(lua_touserdata(L, n));
		}
	};

	SceneGraph::Node* luaR_tonode(lua_State *L, const int n)
	{
		SceneGraph::Node *node =
		    luaR_cast<SceneGraph::Node, Rainbow::Lua::SceneGraph::kCastingUnsafe>::from(L, n);
		LUA_CHECK(L, node, "rainbow.scenegraph: Invalid node specified");
		return node;
	}
}

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char SceneGraph::Bind::class_name[] = "scenegraph";

	template<>
	const bool SceneGraph::Bind::is_constructible = false;

	template<>
	const luaL_Reg SceneGraph::Bind::functions[] = {
		{ "add_animation",   &SceneGraph::add_animation },
		{ "add_batch",       &SceneGraph::add_batch },
		{ "add_drawable",    &SceneGraph::add_drawable },
		{ "add_label",       &SceneGraph::add_label },
		{ "add_node",        &SceneGraph::add_node },
		{ "attach_program",  &SceneGraph::attach_program },
		{ "disable",         &SceneGraph::disable },
		{ "enable",          &SceneGraph::enable },
		{ "remove",          &SceneGraph::remove },
		{ "set_parent",      &SceneGraph::set_parent },
		{ "move",            &SceneGraph::move },
		{ nullptr, nullptr }
	};

	SceneGraph* SceneGraph::create(lua_State *L, ::SceneGraph::Node *root)
	{
		lua_pushlstring(L, class_name, sizeof(class_name) / sizeof(char) - 1);
		void *data = lua_newuserdata(L, sizeof(SceneGraph));
		luaL_newmetatable(L, class_name);  // metatable = {}
		luaL_setfuncs(L, functions, 0);
		luaR_rawsetcclosurefield(L, &tostring<SceneGraph>, "__tostring");
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_rawset(L, -3);  // metatable.__index = metatable
		lua_pushliteral(L, "__metatable");
		lua_createtable(L, 0, 0);
		lua_rawset(L, -3);  // metatable.__metatable = {}
		lua_setmetatable(L, -2);
		lua_rawset(L, -3);

		return static_cast<SceneGraph*>(new (data) SceneGraph(root));
	}

	void SceneGraph::destroy(lua_State *L, SceneGraph *scenegraph)
	{
		lua_getglobal(L, "rainbow");
		if (!lua_istable(L, -1))
			return;
		lua_pushlstring(L, class_name, sizeof(class_name) / sizeof(char) - 1);
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1);
		scenegraph->~SceneGraph();
	}

	SceneGraph::SceneGraph(::SceneGraph::Node *root) : node(root) { }

	template<class T, SceneGraph::CastingMethod C>
	int SceneGraph::add_child(lua_State *L)
	{
		R_ASSERT(luaR_isuserdata(L, 2) &&
		         (luaR_isuserdata(L, 3) || lua_isnone(L, 3)),
		         "rainbow.scenegraph: Invalid parameters");

		SceneGraph *self = Bind::self(L);
		if (!self)
			return 0;

		replacetable(L, 2);
		replacetable(L, 3);

		// Retrieve Lua wrapper.
		T *obj = luaR_cast<T, C>::from(L, -1);

		// Retrieve and add element.
		::SceneGraph::Node *node = lua_isnone(L, 3) ? self->node
		                                            : luaR_tonode(L, 2);
		R_ASSERT(node, "This shouldn't ever happen.");
		lua_pushlightuserdata(L, node->add_child(obj->get()));
		return 1;
	}

	int SceneGraph::add_animation(lua_State *L)
	{
		return add_child<Animation, kCastingSafe>(L);
	}

	int SceneGraph::add_batch(lua_State *L)
	{
		return add_child<SpriteBatch, kCastingSafe>(L);
	}

	int SceneGraph::add_drawable(lua_State *L)
	{
		return add_child<Drawable, kCastingUnsafe>(L);
	}

	int SceneGraph::add_label(lua_State *L)
	{
		return add_child<Label, kCastingSafe>(L);
	}

	int SceneGraph::add_node(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2) || lua_isnone(L, 2),
		           "rainbow.scenegraph:add_node([parent])");

		SceneGraph *self = Bind::self(L);
		if (!self)
			return 0;

		::SceneGraph::Node *node = lua_isuserdata(L, 2) ? luaR_tonode(L, 2)
		                                                : self->node;
		R_ASSERT(node, "This shouldn't ever happen.");
		lua_pushlightuserdata(L, node->add_child(new ::SceneGraph::Node()));
		return 1;
	}

	int SceneGraph::attach_program(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2) &&
		           ((lua_isnumber(L, 3) && lua_tointeger(L, 3) == 0) ||
		                lua_isuserdata(L, 3)),
		           "rainbow.scenegraph:attach_program(node, program)");

		SceneGraph *self = Bind::self(L);
		if (!self)
			return 0;

		int program = lua_isuserdata(L, 3) ? luaR_cast<Shader, kCastingUnsafe>::from(L, 3)->id()
		                                   : 0;
		luaR_tonode(L, 2)->attach_program(program);
		return 0;
	}

	int SceneGraph::disable(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "rainbow.scenegraph:disable(node)");

		luaR_tonode(L, 2)->enabled = false;
		return 0;
	}

	int SceneGraph::enable(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "rainbow.scenegraph:enable(node)");

		luaR_tonode(L, 2)->enabled = true;
		return 0;
	}

	int SceneGraph::remove(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "rainbow.scenegraph:remove(node)");

		luaR_tonode(L, 2)->remove();
		return 0;
	}

	int SceneGraph::set_parent(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2) && lua_isuserdata(L, 3),
		           "rainbow.scenegraph:set_parent(parent, child)");

		luaR_tonode(L, 3)->set_parent(luaR_tonode(L, 2));
		return 0;
	}

	int SceneGraph::move(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2) &&
		           lua_isnumber(L, 3) &&
		           lua_isnumber(L, 4),
		           "rainbow.scenegraph:move(node, x, y)");

		luaR_tonode(L, 2)->move(Vec2f(lua_tonumber(L, 3), lua_tonumber(L, 4)));
		return 0;
	}
} NS_RAINBOW_LUA_END
