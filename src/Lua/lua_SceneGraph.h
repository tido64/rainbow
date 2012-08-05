#ifndef LUA_SCENEGRAPH_H_
#define LUA_SCENEGRAPH_H_

#include "LuaMachine.h"
#include "Graphics/SceneGraph.h"

namespace Rainbow
{
	namespace Lua
	{
		class SceneGraph
		{
			friend class ::LuaMachine;

		public:
			static const char class_name[];
			static const LuaMachine::Method<SceneGraph> methods[];

			int add_animation(lua_State *);
			int add_batch(lua_State *);
			int add_label(lua_State *);
			int add_node(lua_State *);

			int enable(lua_State *);
			int disable(lua_State *);
			int remove(lua_State *);
			int set_parent(lua_State *);

			int move(lua_State *);
			int rotate(lua_State *);
			int scale(lua_State *);

		private:
			SceneGraph **ptr;
			::SceneGraph::Node *root;

			SceneGraph(lua_State *, ::SceneGraph::Node *);
			SceneGraph(const SceneGraph &);

			template<class T, class W>
			int add_child(lua_State *);

			inline ::SceneGraph::Node* to_node(lua_State *, const int);

			SceneGraph& operator=(const SceneGraph &);
		};

		template<class T, class W>
		int SceneGraph::add_child(lua_State *L)
		{
			R_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "Invalid parameters");

			// Retrieve Lua wrapper
			const int n = lua_gettop(L);
			LUA_CHECK(L, lua_type(L, n) != LUA_TNIL, "rainbow.scenegraph: Invalid child node specified");
			lua_rawgeti(L, n, 0);
			W **w = static_cast<W**>(luaL_checkudata(L, -1, W::class_name));

			// Retrieve and add element
			T *s = (*w)->raw_ptr();
			::SceneGraph::Node *node = (n & 1) ? this->root : this->to_node(L, 1);

			// Clean up and return node
			lua_pop(L, 1);
			lua_pushlightuserdata(L, node->add_child(s));
			return 1;
		}

		::SceneGraph::Node* SceneGraph::to_node(lua_State *L, const int n)
		{
			::SceneGraph::Node *node = static_cast< ::SceneGraph::Node*>(lua_touserdata(L, n));
			LUA_CHECK(L, node, "rainbow.scenegraph: Invalid node specified");
			return node;
		}
	}
}

#endif
