// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_RenderQueue.h"

#include "Graphics/Drawable.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"

namespace
{
    template <typename T>
    auto unchecked_cast(lua_State* L, int n)
    {
        return static_cast<T*>(lua_touserdata(L, n));
    }
}

namespace rainbow { namespace lua
{
    constexpr bool RenderQueue::is_constructible;

    const char RenderQueue::class_name[] = "renderqueue";

    const luaL_Reg RenderQueue::functions[]{
        {"add",                       &RenderQueue::add},
        {"disable",                   &RenderQueue::disable},
        {"enable",                    &RenderQueue::enable},
        {"erase",                     &RenderQueue::erase},
        {"erase_if_tag_starts_with",  &RenderQueue::erase_if_tag_starts_with},
        {"insert",                    &RenderQueue::insert},
        {"set_program",               &RenderQueue::set_program},
        {"set_tag",                   &RenderQueue::set_tag},
        {nullptr,                     nullptr}};

    template <typename F>
    auto RenderQueue::invoke(lua_State* L, F&& func) -> int
    {
        RenderQueue* self = Bind::self(L);
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

        RenderQueue* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        return self->emplace(self->render_queue_->cend(), L, 2);
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

        RenderQueue* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        auto tag = lua_tostring(L, 2);
        remove_if(*self->render_queue_, [tag](auto&& unit) {
            return unit.tag().find(tag) == 0;
        });
        return 0;
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

    auto RenderQueue::emplace(graphics::RenderQueue::const_iterator pos,
                              lua_State* L,
                              int index) -> int
    {
        auto iter = render_queue_->end();
        replacetable(L, index);
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
}}
