// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/Helper.h"

#include "Graphics/TextureAtlas.h"
#include "Input/VirtualKey.h"

using rainbow::Animation;
using rainbow::Color;
using rainbow::SharedPtr;
using rainbow::TextureAtlas;
using rainbow::Vec2f;

#define PUSH_OBJECT() const auto obj_idx = duk_push_object(ctx)
#define PUT_PROP(obj, prop)                                                    \
    do                                                                         \
    {                                                                          \
        duk::push(ctx, obj.prop);                                              \
        duk::put_prop_literal(ctx, obj_idx, #prop);                            \
    } while (false)

namespace rainbow::duk
{
    template <typename T>
    auto get_shared(duk_context* ctx, duk_idx_t idx) -> SharedPtr<T>
    {
        duk_require_object(ctx, idx);
        auto ptr = static_cast<SharedPtr<T>*>(duk::push_instance(ctx, idx));
        duk_pop(ctx);
        return *ptr;
    }

    void push(duk_context* ctx, VirtualKey key)
    {
        duk_push_int(ctx, to_underlying_type(key));
    }

    template <typename T, size_t N>
    void push_tagged_pointer(duk_context* ctx, T ptr, const char (&tag)[N])
    {
        const auto obj_idx = duk_push_bare_object(ctx);
        duk::push(ctx,
                  const_cast<void*>(
                      type_id<std::remove_pointer_t<decltype(ptr)>>().value()));
        duk::put_prop_literal(ctx, obj_idx, DUKR_HIDDEN_SYMBOL_TYPE);
        duk::put_instance(ctx, obj_idx, ptr);
        duk::push_literal(ctx, tag);
        duk::put_prop_literal(ctx, obj_idx, DUKR_WELLKNOWN_SYMBOL_TOSTRINGTAG);
        duk_freeze(ctx, obj_idx);
    }
}  // namespace rainbow::duk

void rainbow::duk::dump_context(duk_context* ctx)
{
    if (duk_is_error(ctx, -1))
    {
        duk::get_prop_literal(ctx, -1, "stack");
        LOGF("JavaScript: %s", duk_safe_to_string(ctx, -1));
        duk_pop(ctx);
    }
    else
    {
        LOGF("JavaScript: %s", duk_safe_to_string(ctx, -1));
    }

    duk_push_context_dump(ctx);
    LOGF("JavaScript: %s", duk_to_string(ctx, -1));
    duk_pop(ctx);
}

template <>
auto rainbow::duk::get<Animation::Callback>(duk_context* ctx, duk_idx_t idx)
    -> Animation::Callback
{
    if (!duk_is_object(ctx, idx))
    {
        duk_push_undefined(ctx);
        duk::put_prop_literal(ctx, -2, DUKR_HIDDEN_SYMBOL_CALLBACK);
        return {};
    }

    duk_dup(ctx, idx);
    duk::put_prop_literal(ctx, -2, DUKR_HIDDEN_SYMBOL_CALLBACK);
    return [ctx, ptr = duk_get_heapptr(ctx, -1)](
               Animation*, AnimationEvent event) {
        duk_push_heapptr(ctx, ptr);
        // => [ animation ]
        duk::get_prop_literal(ctx, -1, DUKR_HIDDEN_SYMBOL_CALLBACK);
        // => [ animation callback ]
        duk_dup(ctx, -2);
        // => [ animation callback animation ]
        duk::push(ctx, to_underlying_type(event));
        // => [ animation callback animation event ]
        duk_pcall(ctx, 2);
        // => [ animation ]
        duk_pop(ctx);
    };
}

template <>
auto rainbow::duk::get<Animation::Frames>(duk_context* ctx, duk_idx_t idx)
    -> Animation::Frames
{
    auto size = duk_get_length(ctx, idx);
    auto frames = std::make_unique<Animation::Frame[]>(size + 1);

    for (duk_uarridx_t i = 0; i < size; ++i)
    {
        duk_get_prop_index(ctx, idx, i);
        frames[i] = duk_require_uint(ctx, -1);
        duk_pop(ctx);
    }

    frames[size] = Animation::Frame::end();
    return frames;
}

template <>
auto rainbow::duk::get<Color>(duk_context* ctx, duk_idx_t idx) -> Color
{
    duk::get_prop_literal(ctx, idx, "r");
    duk::get_prop_literal(ctx, idx, "g");
    duk::get_prop_literal(ctx, idx, "b");
    duk::get_prop_literal(ctx, idx, "a");

    Color c{narrow_cast<uint8_t>(duk_require_int(ctx, -4)),
            narrow_cast<uint8_t>(duk_require_int(ctx, -3)),
            narrow_cast<uint8_t>(duk_require_int(ctx, -2)),
            narrow_cast<uint8_t>(duk_require_int(ctx, -1))};

    duk_pop_n(ctx, 4);
    return c;
}

template <>
auto rainbow::duk::get<SharedPtr<TextureAtlas>>(duk_context* ctx, duk_idx_t idx)
    -> SharedPtr<TextureAtlas>
{
    return get_shared<TextureAtlas>(ctx, idx);
}

template <>
auto rainbow::duk::get<Vec2f>(duk_context* ctx, duk_idx_t idx) -> Vec2f
{
    duk::get_prop_literal(ctx, idx, "x");
    duk::get_prop_literal(ctx, idx, "y");

    Vec2f v{narrow_cast<float>(duk_require_number(ctx, -2)),
            narrow_cast<float>(duk_require_number(ctx, -1))};

    duk_pop_2(ctx);
    return v;
}

template <>
auto rainbow::duk::get<rainbow::audio::Channel*>(duk_context* ctx,
                                                 duk_idx_t idx)
    -> rainbow::audio::Channel*
{
    return get_pointer<audio::Channel>(ctx, idx);
}

template <>
auto rainbow::duk::get<rainbow::audio::Sound*>(duk_context* ctx, duk_idx_t idx)
    -> rainbow::audio::Sound*
{
    return get_pointer<audio::Sound>(ctx, idx);
}

void rainbow::duk::push(duk_context* ctx, const Color& c)
{
    PUSH_OBJECT();
    PUT_PROP(c, r);
    PUT_PROP(c, g);
    PUT_PROP(c, b);
    PUT_PROP(c, a);
}

void rainbow::duk::push(duk_context* ctx, const SpriteRef& ref)
{
    const auto obj_idx = duk_push_bare_object(ctx);
    auto buf = duk_push_fixed_buffer(ctx, sizeof(SpriteRef));
    new (buf) SpriteRef(ref);
    duk::put_prop_literal(ctx, obj_idx, DUKR_HIDDEN_SYMBOL_ADDRESS);

    duk_push_global_stash(ctx);
    duk_get_prop_index(ctx, -1, DUKR_IDX_SPRITE_PROTOTYPE);
    duk_set_prototype(ctx, obj_idx);
    duk_freeze(ctx, obj_idx);

    duk_pop(ctx);

    R_ASSERT(duk_get_top(ctx) == obj_idx + 1, "We didn't clean up properly!");
}

void rainbow::duk::push(duk_context* ctx, const Vec2f& v)
{
    PUSH_OBJECT();
    PUT_PROP(v, x);
    PUT_PROP(v, y);
}

void rainbow::duk::push(duk_context* ctx, audio::Channel* channel)
{
    push_tagged_pointer(ctx, channel, "Rainbow.Channel");
}

void rainbow::duk::push(duk_context* ctx, audio::Sound* sound)
{
    push_tagged_pointer(ctx, sound, "Rainbow.Sound");
}
