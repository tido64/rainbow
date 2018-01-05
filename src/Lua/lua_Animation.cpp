// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Animation.h"

#include "Lua/lua_Sprite.h"

using rainbow::lua::Animation;

namespace
{
    constexpr char kErrorHandlingAnimationStateEvent[] =
        "An error occurred while handling an animation state event";

    auto get_frames(lua_State* L, int n)
    {
        const size_t count = lua_rawlen(L, n);
        auto frames = std::make_unique<rainbow::Animation::Frame[]>(count + 1);
        int i = 0;
        std::generate_n(frames.get(),
                        count,
                        [L, n, &i]
                        {
                            lua_rawgeti(L, n, ++i);
                            return lua_tointeger(L, -1);
                        });
        lua_pop(L, i);
        frames[count] = rainbow::Animation::Frame::end();
        return frames;
    }
}

Animation::Animation(lua_State* L)
{
    // rainbow.animation(<sprite>, frames{}, fps, loop_delay = 0)
    checkargs<nil_or<Sprite>, void*, lua_Number, nil_or<lua_Number>>(L);

    SpriteRef sprite;
    unwrapuserdata(L, 1);
    if (isuserdata(L, 1))
        sprite = touserdata<Sprite>(L, 1)->get();
    animation_ = std::make_unique<rainbow::Animation>(
        sprite, get_frames(L, 2), lua_tointeger(L, 3), optinteger(L, 4, 0));
}

auto Animation::current_frame(lua_State* L) -> int
{
    return get1i(L, [](rainbow::Animation* animation) {
        return animation->current_frame();
    });
}

auto Animation::is_stopped(lua_State* L) -> int
{
    return get1b(L, [](const rainbow::Animation* animation) {
        return animation->is_stopped();
    });
}

auto Animation::set_delay(lua_State* L) -> int
{
    // <animation>:set_delay(delay_in_ms)
    return set1i(L, [](rainbow::Animation* animation, int delay) {
        animation->set_delay(delay);
    });
}

auto Animation::set_frame_rate(lua_State* L) -> int
{
    // <animation>:set_frame_rate(fps)
    return set1i(L, [](rainbow::Animation* animation, int fps) {
        animation->set_frame_rate(fps);
    });
}

auto Animation::set_frames(lua_State* L) -> int
{
    // <animation>:set_frames({f1, f2, ...})
    checkargs<Animation, void*>(L);

    return with_self(L, [](Animation* self, lua_State* L) {
        self->animation_->set_frames(get_frames(L, 2));
        return 0;
    });
}

auto Animation::set_listener(lua_State* L) -> int
{
    checkargs<Animation, nil_or<void*>>(L);

    return with_self(L, [](Animation* self, lua_State* L) {
        if (!lua_istable(L, 2))
        {
            self->listener_.reset();
            self->animation_->set_callback(nullptr);
            return 0;
        }

        lua_settop(L, 2);
        self->listener_.reset(L);
        self->animation_->set_callback(
            [L, self](rainbow::Animation*, AnimationEvent e) {
                self->listener_.get();
                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);
                    self->animation_->set_callback(nullptr);
                    return;
                }

                switch (e)
                {
                    case AnimationEvent::Start:
                        lua_getfield(L, -1, "on_animation_start");
                        break;
                    case AnimationEvent::End:
                        lua_getfield(L, -1, "on_animation_end");
                        break;
                    case AnimationEvent::Complete:
                        lua_getfield(L, -1, "on_animation_complete");
                        break;
                    case AnimationEvent::Frame:
                        lua_getfield(L, -1, "on_animation_frame");
                        break;
                }
                if (!lua_isfunction(L, -1))
                {
                    lua_pop(L, 2);
                    return;
                }

                lua_insert(L, -2);
                call(L, 1, 0, 0, kErrorHandlingAnimationStateEvent);
            });

        return 0;
    });
}

auto Animation::set_sprite(lua_State* L) -> int
{
    // <animation>:set_sprite(<sprite>)
    return set1ud<Sprite>(
        L,
        [](rainbow::Animation* animation, const SpriteRef& sprite) {
            animation->set_sprite(sprite);
        });
}

auto Animation::jump_to(lua_State* L) -> int
{
    return set1i(L, [](rainbow::Animation* animation, int frame) {
        animation->jump_to(static_cast<unsigned int>(frame));
    });
}

auto Animation::rewind(lua_State* L) -> int
{
    return with_self(L, [](Animation* self, lua_State*) {
        self->animation_->rewind();
        return 0;
    });
}

auto Animation::start(lua_State* L) -> int
{
    return with_self(L, [](Animation* self, lua_State*) {
        self->animation_->start();
        return 0;
    });
}

auto Animation::stop(lua_State* L) -> int
{
    return with_self(L, [](Animation* self, lua_State*) {
        self->animation_->stop();
        return 0;
    });
}

LUA_REG_OBJECT(Animation, "animation") {
    LUA_REG_OBJECT_FUNC(Animation, current_frame),
    LUA_REG_OBJECT_FUNC(Animation, is_stopped),
    LUA_REG_OBJECT_FUNC(Animation, set_delay),
    LUA_REG_OBJECT_FUNC(Animation, set_frame_rate),
    LUA_REG_OBJECT_FUNC(Animation, set_frames),
    LUA_REG_OBJECT_FUNC(Animation, set_listener),
    LUA_REG_OBJECT_FUNC(Animation, set_sprite),
    LUA_REG_OBJECT_FUNC(Animation, jump_to),
    LUA_REG_OBJECT_FUNC(Animation, rewind),
    LUA_REG_OBJECT_FUNC(Animation, start),
    LUA_REG_OBJECT_FUNC(Animation, stop),
    LUA_REG_OBJECT_END()
};
