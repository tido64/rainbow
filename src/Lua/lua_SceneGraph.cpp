// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_SceneGraph.h"

#include <unordered_set>

#include "Graphics/Drawable.h"
#include "Graphics/SceneGraph.h"
#include "Lua/LuaSyntax.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

using SceneGraph::Node;

namespace
{
#ifndef NDEBUG
	std::unordered_set<Node*> g_nodes;

	bool is_valid_node(lua_State *L, const int n, Node *node)
	{
		if (!node)
			return luaL_argerror(L, n, "invalid node");
		if (!g_nodes.count(node))
			return luaL_argerror(L, n, "non-existing node");
		return true;
	}

	bool register_node(Node *node)
	{
		return g_nodes.insert(node).second;
	}

	bool unregister_node(Node *node)
	{
		node->for_each([](Node *node) { g_nodes.erase(node); });
		return true;
	}
#endif

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

	Node* luaR_tonode(lua_State *L, const int n)
	{
		Node *node =
		    luaR_cast<Node, Rainbow::Lua::SceneGraph::kCastingUnsafe>::
		        from(L, n);
		R_ASSERT(is_valid_node(L, n, node), "Invalid or non-existing node");
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

	SceneGraph* SceneGraph::create(lua_State *L, Node *root)
	{
		R_ASSERT(register_node(root), "Failed to register root node");

		lua_pushlstring(L, class_name, strllen(class_name));
		void *data = lua_newuserdata(L, sizeof(SceneGraph));
		luaL_newmetatable(L, class_name);  // metatable = {}
		luaL_setfuncs(L, functions, 0);
		luaR_rawsetcfunction(L, "__tostring", &tostring<SceneGraph>);
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
		lua_pushlstring(L, class_name, strllen(class_name));
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_pop(L, 1);
		scenegraph->~SceneGraph();
	}

	SceneGraph::SceneGraph(Node *root) : node(root) { }

	template<class T, SceneGraph::CastingMethod C>
	int SceneGraph::add_child(lua_State *L)
	{
		SceneGraph *self = Bind::self(L);
		if (!self)
			return 0;

		Node *node;
		int obj;
		if (lua_isnoneornil(L, 3))
		{
			Argument<T>::is_required(L, 2);
			node = self->node;
			obj = 2;
		}
		else
		{
			Argument<Node>::is_required(L, 2);
			Argument<T>::is_required(L, 3);
			node = luaR_tonode(L, 2);
			obj = 3;
		}
		replacetable(L, obj);
		Node *child = node->add_child(luaR_cast<T, C>::from(L, obj)->get());
		R_ASSERT(register_node(child), "Failed to register node");
		lua_pushlightuserdata(L, child);
		return 1;
	}

	int SceneGraph::add_animation(lua_State *L)
	{
		// rainbow.scenegraph:add_animation([node], <animation>)
		return add_child<Animation, kCastingSafe>(L);
	}

	int SceneGraph::add_batch(lua_State *L)
	{
		// rainbow.scenegraph:add_batch([node], <batch>)
		return add_child<SpriteBatch, kCastingSafe>(L);
	}

	int SceneGraph::add_drawable(lua_State *L)
	{
		// rainbow.scenegraph:add_drawable([node], <drawable>)
		return add_child<Drawable, kCastingUnsafe>(L);
	}

	int SceneGraph::add_label(lua_State *L)
	{
		// rainbow.scenegraph:add_label([node], <label>)
		return add_child<Label, kCastingSafe>(L);
	}

	int SceneGraph::add_node(lua_State *L)
	{
		// rainbow.scenegraph:add_node([parent])
		Argument<Node>::is_optional(L, 2);

		SceneGraph *self = Bind::self(L);
		if (!self)
			return 0;

		Node *node = lua_isuserdata(L, 2) ? luaR_tonode(L, 2) : self->node;
		R_ASSERT(node, "This shouldn't ever happen.");
		Node *child = new Node();
		R_ASSERT(register_node(child), "Failed to register node");
		lua_pushlightuserdata(L, node->add_child(child));
		return 1;
	}

	int SceneGraph::attach_program(lua_State *L)
	{
		// rainbow.scenegraph:attach_program(node, program)
		Argument<Node>::is_required(L, 2);
		Argument<Shader>::is_required(L, 3);

		SceneGraph *self = Bind::self(L);
		if (!self)
			return 0;

		int program = lua_isuserdata(L, 3)
		    ? luaR_cast<Shader, kCastingUnsafe>::from(L, 3)->id()
		    : 0;
		luaR_tonode(L, 2)->attach_program(program);
		return 0;
	}

	int SceneGraph::disable(lua_State *L)
	{
		// rainbow.scenegraph:disable(node)
		Argument<Node>::is_required(L, 2);

		luaR_tonode(L, 2)->enabled = false;
		return 0;
	}

	int SceneGraph::enable(lua_State *L)
	{
		// rainbow.scenegraph:enable(node)
		Argument<Node>::is_required(L, 2);

		luaR_tonode(L, 2)->enabled = true;
		return 0;
	}

	int SceneGraph::remove(lua_State *L)
	{
		// rainbow.scenegraph:remove(node)
		Argument<Node>::is_required(L, 2);

		Node *node = luaR_tonode(L, 2);
		R_ASSERT(unregister_node(node), "Failed to unregister node");
		node->remove();
		return 0;
	}

	int SceneGraph::set_parent(lua_State *L)
	{
		// rainbow.scenegraph:set_parent(parent, child)
		Argument<Node>::is_required(L, 2);
		Argument<Node>::is_required(L, 3);

		luaR_tonode(L, 2)->add_child(luaR_tonode(L, 3));
		return 0;
	}

	int SceneGraph::move(lua_State *L)
	{
		// rainbow.scenegraph:move(node, x, y)
		Argument<Node>::is_required(L, 2);
		Argument<lua_Number>::is_required(L, 3);
		Argument<lua_Number>::is_required(L, 4);

		const Vec2f delta(lua_tonumber(L, 3), lua_tonumber(L, 4));
		if (delta.is_zero())
			return 0;

		luaR_tonode(L, 2)->move(delta);
		return 0;
	}
} NS_RAINBOW_LUA_END
