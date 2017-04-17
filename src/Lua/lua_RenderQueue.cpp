// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_RenderQueue.h"

#include "Graphics/Drawable.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

using rainbow::lua::RenderQueue;

namespace
{
    template <typename T>
    auto unchecked_cast(lua_State* L, int n)
    {
        return static_cast<T*>(lua_touserdata(L, n));
    }
}

template <typename F>
auto RenderQueue::invoke(lua_State* L, F&& func) -> int
{
    RenderQueue* self = touserdata<RenderQueue>(L, 1);
    if (self == nullptr)
        return 0;

    auto&& render_queue = *self->render_queue_;
    int index;
    const int type = lua_type(L, 2);
    if (type == LUA_TNUMBER)
    {
        // Subtract 1 from index to be consistent with Lua's base index.
        index = lua_tointeger(L, 2) - 1;
        if (index < 0 || index >= static_cast<int>(render_queue.size()))
            return 0;
    }
    else if (type == LUA_TSTRING)
    {
        const auto result = std::find_if(
            render_queue.begin(),
            render_queue.end(),
            [tag = lua_tostring(L, 2)](const graphics::RenderUnit& unit) {
                return unit.tag() == tag;
            });

        if (result == render_queue.end())
            return 0;

        index = result - render_queue.begin();
    }
    else
    {
        luaL_argerror(L, 2, "number or string expected");
        return 0;
    }

    return func(L, *self, index);
}

auto RenderQueue::add(lua_State* L) -> int
{
    // rainbow.renderqueue:add(<drawable>[, tag])
    checkargs<RenderQueue, IDrawable, nil_or<char*>>(L);

    return with_self(L, [](RenderQueue* self, lua_State* L) {
        return self->emplace(self->render_queue_->cend(), L, 2);
    });
}

auto RenderQueue::disable(lua_State* L) -> int
{
    // rainbow.renderqueue:disable(unit|tag)
    checkargs<RenderQueue, char*>(L);

    return invoke(L, [](lua_State*, RenderQueue& self, int index) {
        (*self.render_queue_)[index].disable();
        return 0;
    });
}

auto RenderQueue::enable(lua_State* L) -> int
{
    // rainbow.renderqueue:enable(unit|tag)
    checkargs<RenderQueue, char*>(L);

    return invoke(L, [](lua_State*, RenderQueue& self, int index) {
        (*self.render_queue_)[index].enable();
        return 0;
    });
}

auto RenderQueue::erase(lua_State* L) -> int
{
    // rainbow.renderqueue:erase(unit|tag)
    checkargs<RenderQueue, char*>(L);

    return invoke(L, [](lua_State*, RenderQueue& self, int index) {
        self.render_queue_->erase(self.render_queue_->cbegin() + index);
        return 0;
    });
}

auto RenderQueue::erase_if_tag_starts_with(lua_State* L) -> int
{
    // rainbow.renderqueue:erase_if_tag_starts_with(tag)
    checkargs<RenderQueue, char*>(L);

    return with_self(L, [](RenderQueue* self, lua_State* L) {
        auto tag = lua_tostring(L, 2);
        remove_if(*self->render_queue_, [tag](auto&& unit) {
            return unit.tag().find(tag) == 0;
        });
        return 0;
    });
}

auto RenderQueue::insert(lua_State* L) -> int
{
    // rainbow.renderqueue:insert(pos, <drawable>)
    checkargs<RenderQueue, lua_Number, IDrawable>(L);

    return invoke(L, [](lua_State* L, RenderQueue& self, int index) {
        return self.emplace(self.render_queue_->cbegin() + index, L, 3);
    });
}

auto RenderQueue::set_program(lua_State* L) -> int
{
    // rainbow.renderqueue:set_program(unit|tag, program)
    checkargs<RenderQueue, char*, Shader>(L);

    return invoke(L, [](lua_State* L, RenderQueue& self, int index) {
        (*self.render_queue_)[index].set_program(
            isuserdata(L, 3) ? unchecked_cast<Shader>(L, 3)->id() : 0);
        return 0;
    });
}

auto RenderQueue::set_tag(lua_State* L) -> int
{
    // rainbow.renderqueue:set_tag(unit, tag)
    checkargs<RenderQueue, lua_Number, char*>(L);

    return invoke(L, [](lua_State* L, RenderQueue& self, int index) {
        (*self.render_queue_)[index].set_tag(lua_tostring(L, 3));
        return 0;
    });
}

auto RenderQueue::emplace(rainbow::graphics::RenderQueue::const_iterator pos,
                          lua_State* L,
                          int index) -> int
{
    auto iter = render_queue_->end();
    unwrapuserdata(L, index);
    if (auto animation = tryuserdata<Animation>(L, index))
    {
        iter = render_queue_->emplace(pos, *animation->get());
    }
    else if (auto label = tryuserdata<Label>(L, index))
    {
        iter = render_queue_->emplace(pos, *label->get());
    }
    else if (auto batch = tryuserdata<SpriteBatch>(L, index))
    {
        iter = render_queue_->emplace(pos, *batch->get());
    }
    else if (auto drawable = unchecked_cast<IDrawable>(L, index))
    {
        iter = render_queue_->emplace(pos, *drawable->get());
    }
    else
    {
        return 0;
    }

    if (lua_isstring(L, ++index))
        iter->set_tag(lua_tostring(L, index));

    // Add 1 to index to be consistent with Lua's base index.
    lua_pushinteger(
        L, static_cast<lua_Integer>(iter - render_queue_->begin()) + 1);
    return 1;
}

LUA_REG_OBJECT(RenderQueue, "renderqueue") {
    LUA_REG_OBJECT_FUNC(RenderQueue, add),
    LUA_REG_OBJECT_FUNC(RenderQueue, disable),
    LUA_REG_OBJECT_FUNC(RenderQueue, enable),
    LUA_REG_OBJECT_FUNC(RenderQueue, erase),
    LUA_REG_OBJECT_FUNC(RenderQueue, erase_if_tag_starts_with),
    LUA_REG_OBJECT_FUNC(RenderQueue, insert),
    LUA_REG_OBJECT_FUNC(RenderQueue, set_program),
    LUA_REG_OBJECT_FUNC(RenderQueue, set_tag),
    LUA_REG_OBJECT_END()
};
