#ifndef LUA_SCENEGRAPH_H_
#define LUA_SCENEGRAPH_H_

#include "Graphics/SceneGraph.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaMachine.h"

namespace Rainbow
{
	namespace Lua
	{
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

		private:
			::SceneGraph::Node *root;

			SceneGraph(lua_State *, ::SceneGraph::Node *);

			void unregister(lua_State *);
		};
	}
}

#endif
