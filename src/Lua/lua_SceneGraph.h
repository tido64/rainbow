#ifndef LUA_SCENEGRAPH_H_
#define LUA_SCENEGRAPH_H_

#include "LuaMachine.h"
#include "Graphics/SceneGraph.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace Helper
		{
			template<class T, bool unsafe>
			struct ToUserData;

			template<class T>
			struct ToUserData<T, false>
			{
				static inline T* Cast(lua_State *L, const int n)
				{
					return *static_cast<T**>(luaL_checkudata(L, n, T::class_name));
				}
			};

			template<class T>
			struct ToUserData<T, true>
			{
				static inline T* Cast(lua_State *L, const int n)
				{
					return *static_cast<T**>(lua_touserdata(L, n));
				}
			};
		}

		class SceneGraph : private NonCopyable<SceneGraph>
		{
			friend class Rainbow::LuaMachine;

		public:
			static const char class_name[];
			static const Method<SceneGraph> methods[];

			int add_animation(lua_State *);
			int add_batch(lua_State *);
			int add_drawable(lua_State *);
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
			::SceneGraph::Node *root;

			SceneGraph(lua_State *, ::SceneGraph::Node *);

			template<class T, bool unsafe>
			int add_child(lua_State *);

			inline ::SceneGraph::Node* to_node(lua_State *, const int);
		};

		template<class T, bool unsafe>
		int SceneGraph::add_child(lua_State *L)
		{
			R_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "Invalid parameters");

			// Ensure it's not a nil value.
			const int n = lua_gettop(L);
			LUA_CHECK(L, lua_type(L, n) != LUA_TNIL, "rainbow.scenegraph: Invalid child node specified");

			// Retrieve Lua wrapper.
			lua_rawgeti(L, n, 0);
			T *obj = Helper::ToUserData<T, unsafe>::Cast(L, -1);
			lua_pop(L, 1);

			// Retrieve and add element.
			::SceneGraph::Node *node = (n & 1) ? this->root : this->to_node(L, 1);
			lua_pushlightuserdata(L, node->add_child(obj->raw_ptr()));

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
