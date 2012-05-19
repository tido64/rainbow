#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_SceneGraph.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

const char lua_SceneGraph::class_name[] = "scenegraph";
const LuaMachine::Method<lua_SceneGraph> lua_SceneGraph::methods[] = {
	{ "add_animation",  &lua_SceneGraph::add_animation },
	{ "add_batch",      &lua_SceneGraph::add_batch },
	{ "add_label",      &lua_SceneGraph::add_label },
	{ "add_node",       &lua_SceneGraph::add_node },
	{ "enable",         &lua_SceneGraph::enable },
	{ "disable",        &lua_SceneGraph::disable },
	{ "remove",         &lua_SceneGraph::remove },
	{ "set_parent",     &lua_SceneGraph::set_parent },
	{ "move",           &lua_SceneGraph::move },
	{ "rotate",         &lua_SceneGraph::rotate },
	{ "scale",          &lua_SceneGraph::scale },
	{ 0, 0 }
};

int lua_SceneGraph::add_animation(lua_State *L)
{
	return this->add_child<Animation, lua_Animation>(L);
}

int lua_SceneGraph::add_batch(lua_State *L)
{
	return this->add_child<SpriteBatch, lua_SpriteBatch>(L);
}

int lua_SceneGraph::add_label(lua_State *L)
{
	return this->add_child<Label, lua_Label>(L);
}

int lua_SceneGraph::add_node(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 0 || lua_gettop(L) == 1, "rainbow.scenegraph:add_node([parent])");

	SceneGraph::Node *node = (!lua_gettop(L)) ? this->root : this->to_node(L, 1);
	lua_pushlightuserdata(L, node->add_child(new SceneGraph::Node()));
	return 1;
}

int lua_SceneGraph::enable(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:enable(node)");

	this->to_node(L, 1)->enabled = true;
	return 0;
}

int lua_SceneGraph::disable(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1, "rainbow.scenegraph:disable(node)");

	this->to_node(L, 1)->enabled = false;
	return 0;
}

int lua_SceneGraph::remove(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "rainbow.scenegraph:remove([parent,] node)");

	const int top = lua_gettop(L);
	SceneGraph::Node *node = (top == 1) ? this->root : this->to_node(L, 1);
	node->remove_child(this->to_node(L, top));
	return 0;
}

int lua_SceneGraph::set_parent(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 2, "rainbow.scenegraph:set_parent(parent, child)");

	SceneGraph::Node *child = this->to_node(L, 2);
	child->set_parent(this->to_node(L, 1));
	return 0;
}

int lua_SceneGraph::move(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 3, "rainbow.scenegraph:move(node, x, y)");

	SceneGraph::Node *node = this->to_node(L, 1);
	node->move(lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

int lua_SceneGraph::rotate(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 2, "rainbow.scenegraph:rotate(node, r)");

	SceneGraph::Node *node = this->to_node(L, 1);
	node->rotate(lua_tonumber(L, 2));
	return 0;
}

int lua_SceneGraph::scale(lua_State *L)
{
	LUA_ASSERT(lua_gettop(L) == 2, "rainbow.scenegraph:scale(node, f)");

	SceneGraph::Node *node = this->to_node(L, 1);
	node->scale(lua_tonumber(L, 2));
	return 0;
}

lua_SceneGraph::lua_SceneGraph(lua_State *L) : ptr(nullptr), root(nullptr)
{
	lua_createtable(L, 0, 16);
	lua_pushvalue(L, -1);
	lua_setfield(L, -3, class_name);
	lua_pushnumber(L, 0);

	this->ptr = static_cast<lua_SceneGraph**>(lua_newuserdata(L, sizeof(this)));
	*this->ptr = this;

	luaL_newmetatable(L, class_name);
	lua_setmetatable(L, -2);
	lua_settable(L, -3);

	for (int i = 0; methods[i].name; ++i)
	{
		lua_pushstring(L, methods[i].name);
		lua_pushnumber(L, i);
		lua_pushcclosure(L, &LuaMachine::thunk<lua_SceneGraph>, 1);
		lua_settable(L, -3);
	}

	lua_pop(L, 1);
}
