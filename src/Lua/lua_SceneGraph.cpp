// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaHelper.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_SceneGraph.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

namespace
{
	enum CastingMethod
	{
		kCastingUnsafe,
		kCastingSafe
	};

	template<class T, CastingMethod C>
	struct luaR_cast;

	template<class T>
	struct luaR_cast<T, kCastingSafe>
	{
		static T* from(lua_State *L, const int n)
		{
			return static_cast<T*>(luaL_checkudata(L, n, T::class_name));
		}
	};

	template<class T>
	struct luaR_cast<T, kCastingUnsafe>
	{
		static T* from(lua_State *L, const int n)
		{
			return static_cast<T*>(lua_touserdata(L, n));
		}
	};

	SceneGraph::Node* luaR_tonode(lua_State *L, const int n)
	{
		SceneGraph::Node *node = luaR_cast<SceneGraph::Node, kCastingUnsafe>::from(L, n);
		LUA_CHECK(L, node, "rainbow.scenegraph: Invalid node specified");
		return node;
	}

	template<class T, CastingMethod C>
	int add_child(SceneGraph::Node *root, lua_State *L)
	{
		R_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2,
		         "Invalid parameters");

		// Ensure it's not a nil value.
		const int n = lua_gettop(L);
		LUA_CHECK(L, lua_type(L, n) != LUA_TNIL,
		          "rainbow.scenegraph: Invalid child node specified");

		// Retrieve Lua wrapper.
		lua_rawgeti(L, n, 0);
		T *obj = luaR_cast<T, C>::from(L, -1);
		lua_pop(L, 1);

		// Retrieve and add element.
		SceneGraph::Node *node = (n == 1) ? root : luaR_tonode(L, 1);
		R_ASSERT(node, "This shouldn't ever happen.");
		lua_pushlightuserdata(L, node->add_child(obj->get()));

		return 1;
	}
}

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<SceneGraph, ::SceneGraph::Node, kBindTypeWeak> LuaSceneGraph;

		template<>
		const char LuaSceneGraph::class_name[] = "scenegraph";

		template<>
		const Method<SceneGraph> LuaSceneGraph::methods[] = {
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
			{ 0, 0 }
		};

		SceneGraph* SceneGraph::create(lua_State *L, ::SceneGraph::Node *root)
		{
			lua_pushlstring(L, class_name, sizeof(class_name) / sizeof(char) - 1);
			lua_createtable(L, 1, 0);  // scenegraph = {}

			SceneGraph *ptr = static_cast<SceneGraph*>(lua_newuserdata(L, sizeof(SceneGraph)));
			luaL_newmetatable(L, class_name);  // mt = {}
			lua_setmetatable(L, -2);           // setmetatable(udata, mt)
			lua_rawseti(L, -2, 0);             // scenegraph[0] = udata

			lua_createtable(L, 0, 2);  // mt = {}
			lua_pushliteral(L, "__index");
			lua_createtable(L, 0, 16);
			for (int i = 0; methods[i].name; ++i)
			{
				lua_pushstring(L, methods[i].name);
				lua_pushnumber(L, i);
				lua_pushcclosure(L, &thunk<SceneGraph>, 1);
				lua_rawset(L, -3);
			}
			lua_rawset(L, -3);        // mt.__index = { .. }
			lua_pushliteral(L, "__metatable");
			lua_createtable(L, 0, 0);
			lua_rawset(L, -3);        // mt.__metatable = {}
			lua_setmetatable(L, -2);  // setmetatable(scenegraph, mt)

			lua_rawset(L, -3);        // rainbow.scenegraph = scenegraph

			return new (ptr) SceneGraph(root);
		}

		void SceneGraph::destroy(lua_State *L, SceneGraph *scenegraph)
		{
			lua_getglobal(L, "rainbow");
			lua_pushlstring(L, class_name, sizeof(class_name) / sizeof(char) - 1);
			lua_pushnil(L);
			lua_rawset(L, -3);
			lua_pop(L, 1);
			scenegraph->~SceneGraph();
		}

		SceneGraph::SceneGraph(::SceneGraph::Node *root) : Bind(root) { }

		int SceneGraph::add_animation(lua_State *L)
		{
			return add_child<Animation, kCastingSafe>(this->ptr, L);
		}

		int SceneGraph::add_batch(lua_State *L)
		{
			return add_child<SpriteBatch, kCastingSafe>(this->ptr, L);
		}

		int SceneGraph::add_drawable(lua_State *L)
		{
			return add_child<Drawable, kCastingUnsafe>(this->ptr, L);
		}

		int SceneGraph::add_label(lua_State *L)
		{
			return add_child<Label, kCastingSafe>(this->ptr, L);
		}

		int SceneGraph::add_node(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 0 || lua_gettop(L) == 1,
			           "rainbow.scenegraph:add_node([parent])");

			::SceneGraph::Node *node = (!lua_gettop(L)) ? this->ptr : luaR_tonode(L, 1);
			R_ASSERT(node, "This shouldn't ever happen.");
			lua_pushlightuserdata(L, node->add_child(new ::SceneGraph::Node()));
			return 1;
		}

		int SceneGraph::attach_program(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2,
			           "rainbow.scenegraph:attach_program(node, program)");

			if ((lua_isnumber(L, 2) && lua_tointeger(L, 2) != 0) || !lua_istable(L, 2))
			{
				R_ERROR("[Rainbow] rainbow.scenegraph:attach_program(): Invalid program");
				return 0;
			}
			int program = 0;
			if (lua_istable(L, 2))
			{
				// Retrieve shader object and get the program id.
				lua_rawgeti(L, 2, 0);  // userdata = table[0]
				program = luaR_cast<Shader, kCastingUnsafe>::from(L, 3)->id();
				lua_pop(L, 1);
			}
			::SceneGraph::Node *node = luaR_tonode(L, 1);
			node->attach_program(program);
			return 0;
		}

		int SceneGraph::disable(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:disable(node)");

			luaR_tonode(L, 1)->enabled = false;
			return 0;
		}

		int SceneGraph::enable(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:enable(node)");

			luaR_tonode(L, 1)->enabled = true;
			return 0;
		}

		int SceneGraph::remove(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:remove(node)");

			luaR_tonode(L, 1)->remove();
			return 0;
		}

		int SceneGraph::set_parent(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2,
			           "rainbow.scenegraph:set_parent(parent, child)");

			::SceneGraph::Node *child = luaR_tonode(L, 2);
			child->set_parent(luaR_tonode(L, 1));
			return 0;
		}

		int SceneGraph::move(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3,
			           "rainbow.scenegraph:move(node, x, y)");

			::SceneGraph::Node *node = luaR_tonode(L, 1);
			const Vec2f delta(luaR_tonumber(L, 2), luaR_tonumber(L, 3));
			node->move(delta);
			return 0;
		}
	}
}
