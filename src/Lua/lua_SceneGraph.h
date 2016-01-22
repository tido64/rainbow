// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SCENEGRAPH_H_
#define LUA_SCENEGRAPH_H_

#include "Common/NonCopyable.h"
#include "Lua/LuaBind.h"

namespace rainbow { class SceneNode; }

NS_RAINBOW_LUA_BEGIN
{
    class SceneGraph : public Bind<SceneGraph>, private NonCopyable<SceneGraph>
    {
        friend Bind;

    public:
        static SceneGraph* create(lua_State*, SceneNode*);
        static void destroy(lua_State*, SceneGraph*);

    private:
        template <typename T, typename F>
        static int add_child(lua_State* L, F&& touserdata);

        static int add_animation(lua_State*);
        static int add_batch(lua_State*);
        static int add_drawable(lua_State*);
        static int add_label(lua_State*);
        static int add_node(lua_State*);
        static int attach_program(lua_State*);
        static int disable(lua_State*);
        static int enable(lua_State*);
        static int remove(lua_State*);
        static int set_parent(lua_State*);
        static int set_tag(lua_State*);
        static int move(lua_State*);

        SceneNode* node_;

        SceneGraph(SceneNode*);
        ~SceneGraph() = default;
    };

    class ScopedNode : public Bind<ScopedNode>
    {
        friend Bind;

    public:
        ScopedNode(lua_State* L);
        ~ScopedNode();

    private:
        SceneNode* node_;
        lua_State* state_;
    };
} NS_RAINBOW_LUA_END

#endif
