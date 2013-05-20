#ifndef LUA_SCENEGRAPH_H_
#define LUA_SCENEGRAPH_H_

#include "Graphics/SceneGraph.h"
#include "Lua/LuaBind.h"
#include "Lua/LuaMachine.h"

namespace Rainbow
{
	namespace Lua
	{
		class SceneGraph :
			public Bind<SceneGraph, ::SceneGraph::Node, kBindTypeWeak>,
			private NonCopyable<SceneGraph>
		{
			friend class Bind<SceneGraph, ::SceneGraph::Node, kBindTypeWeak>;

		public:
			static SceneGraph* create(lua_State *, ::SceneGraph::Node *);
			static void destroy(lua_State *, SceneGraph *);

		private:
			SceneGraph(::SceneGraph::Node *);
			~SceneGraph() { }

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
		};
	}
}

#endif
