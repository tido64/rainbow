#ifndef LUA_SCENEGRAPH_H_
#define LUA_SCENEGRAPH_H_

#include "LuaMachine.h"
#include "Graphics/SceneGraph.h"

class lua_SceneGraph
{
	friend class LuaMachine;

public:
	static const char class_name[];
	static const LuaMachine::Method<lua_SceneGraph> methods[];

	int add_animation(lua_State *);
	int add_batch(lua_State *);
	int add_label(lua_State *);
	int add_node(lua_State *);
	int add_sprite(lua_State *);
	int enable(lua_State *);
	int disable(lua_State *);
	int remove(lua_State *);
	int set_parent(lua_State *);

	int move(lua_State *);
	int rotate(lua_State *);
	int scale(lua_State *);

private:
	lua_SceneGraph **ptr;
	SceneGraph::Node *root;

	lua_SceneGraph(lua_State *);
	lua_SceneGraph(const lua_SceneGraph &);

	template<class T, class W>
	int add_child(lua_State *);

	inline void set_root(SceneGraph::Node *);
	inline SceneGraph::Node* to_node(lua_State *, const int);

	lua_SceneGraph& operator=(const lua_SceneGraph &);
};

template<class T, class W>
int lua_SceneGraph::add_child(lua_State *L)
{
	R_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "add_child: Invalid parameters");

	// Retrieve Lua wrapper
	const int n = lua_gettop(L);
	lua_rawgeti(L, n, 0);
	W **w = static_cast<W**>(luaL_checkudata(L, -1, W::class_name));

	// Retrieve and add element
	T *s = (*w)->raw_ptr();
	SceneGraph::Node *node = (n & 1) ? this->root : this->to_node(L, 1);

	// Clean up and return node
	lua_pop(L, 1);
	lua_pushlightuserdata(L, node->add_child(s));
	return 1;
}

void lua_SceneGraph::set_root(SceneGraph::Node *r)
{
	this->root = r;
}

SceneGraph::Node* lua_SceneGraph::to_node(lua_State *L, const int n)
{
	return static_cast<SceneGraph::Node*>(lua_touserdata(L, n));
}

#endif
