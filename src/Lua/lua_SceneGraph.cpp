#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_SceneGraph.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

namespace Rainbow
{
	namespace Lua
	{
		const char SceneGraph::class_name[] = "scenegraph";
		const Method<SceneGraph> SceneGraph::methods[] = {
			{ "add_animation",  &SceneGraph::add_animation },
			{ "add_batch",      &SceneGraph::add_batch },
			{ "add_drawable",   &SceneGraph::add_drawable },
			{ "add_label",      &SceneGraph::add_label },
			{ "add_node",       &SceneGraph::add_node },
			{ "enable",         &SceneGraph::enable },
			{ "disable",        &SceneGraph::disable },
			{ "remove",         &SceneGraph::remove },
			{ "set_parent",     &SceneGraph::set_parent },
			{ "move",           &SceneGraph::move },
			{ "rotate",         &SceneGraph::rotate },
			{ "scale",          &SceneGraph::scale },
			{ 0, 0 }
		};

		int SceneGraph::add_animation(lua_State *L)
		{
			return this->add_child<Animation, false>(L);
		}

		int SceneGraph::add_batch(lua_State *L)
		{
			return this->add_child<SpriteBatch, false>(L);
		}

		int SceneGraph::add_drawable(lua_State *L)
		{
			return this->add_child<Drawable, true>(L);
		}

		int SceneGraph::add_label(lua_State *L)
		{
			return this->add_child<Label, false>(L);
		}

		int SceneGraph::add_node(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 0 || lua_gettop(L) == 1, "rainbow.scenegraph:add_node([parent])");

			::SceneGraph::Node *node = (!lua_gettop(L)) ? this->root : this->to_node(L, 1);
			lua_pushlightuserdata(L, node->add_child(new ::SceneGraph::Node()));
			return 1;
		}

		int SceneGraph::enable(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:enable(node)");

			this->to_node(L, 1)->enabled = true;
			return 0;
		}

		int SceneGraph::disable(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:disable(node)");

			this->to_node(L, 1)->enabled = false;
			return 0;
		}

		int SceneGraph::remove(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "rainbow.scenegraph:remove([parent,] node)");

			const int top = lua_gettop(L);
			::SceneGraph::Node *node = (top == 1) ? this->root : this->to_node(L, 1);
			node->remove_child(this->to_node(L, top));
			return 0;
		}

		int SceneGraph::set_parent(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "rainbow.scenegraph:set_parent(parent, child)");

			::SceneGraph::Node *child = this->to_node(L, 2);
			child->set_parent(this->to_node(L, 1));
			return 0;
		}

		int SceneGraph::move(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 3, "rainbow.scenegraph:move(node, x, y)");

			::SceneGraph::Node *node = this->to_node(L, 1);
			node->move(lua_tonumber(L, 2), lua_tonumber(L, 3));
			return 0;
		}

		int SceneGraph::rotate(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "rainbow.scenegraph:rotate(node, r)");

			::SceneGraph::Node *node = this->to_node(L, 1);
			node->rotate(lua_tonumber(L, 2));
			return 0;
		}

		int SceneGraph::scale(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "rainbow.scenegraph:scale(node, f)");

			::SceneGraph::Node *node = this->to_node(L, 1);
			node->scale(lua_tonumber(L, 2));
			return 0;
		}

		SceneGraph::SceneGraph(lua_State *L, ::SceneGraph::Node *root) :
			root(root)
		{
			lua_pushlstring(L, class_name, sizeof(class_name) / sizeof(char) - 1);
			lua_createtable(L, 0, 16);
			lua_pushnumber(L, 0);

			SceneGraph **ptr = static_cast<SceneGraph**>(lua_newuserdata(L, sizeof(this)));
			*ptr = this;

			luaL_newmetatable(L, class_name);
			lua_setmetatable(L, -2);
			lua_rawset(L, -3);

			for (int i = 0; methods[i].name; ++i)
			{
				lua_pushstring(L, methods[i].name);
				lua_pushnumber(L, i);
				lua_pushcclosure(L, &thunk<SceneGraph>, 1);
				lua_rawset(L, -3);
			}

			lua_rawset(L, -3);
		}
	}
}
