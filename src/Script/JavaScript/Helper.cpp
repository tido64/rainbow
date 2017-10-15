// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/Helper.h"

#include "Graphics/FontAtlas.h"
#include "Graphics/TextureAtlas.h"
#include "Input/VirtualKey.h"

using rainbow::Animation;
using rainbow::Color;
using rainbow::FontAtlas;
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

#define PUSH_TAGGED_POINTER(ctx, ptr, tag)                                     \
    do                                                                         \
    {                                                                          \
        const auto obj_idx = duk_push_bare_object(ctx);                        \
        duk::push(                                                             \
            ctx,                                                               \
            const_cast<void*>(                                                 \
                type_id<std::remove_pointer_t<decltype(ptr)>>().value()));     \
        duk::put_prop_literal(ctx, obj_idx, DUKR_HIDDEN_SYMBOL_TYPE);          \
        duk::put_instance(ctx, obj_idx, ptr);                                  \
        duk::push_literal(ctx, tag);                                           \
        duk::put_prop_literal(ctx, -2, DUKR_WELLKNOWN_SYMBOL_TOSTRINGTAG);     \
        duk_freeze(ctx, obj_idx);                                              \
    } while (false)

namespace rainbow::duk
{
    template <typename T>
    auto get_shared(duk_context* ctx, duk_idx_t idx) -> SharedPtr<T>
    {
        ScopedStack stack{ctx};

        duk_require_object(ctx, idx);
        return *static_cast<SharedPtr<T>*>(duk::push_instance(ctx, idx));
    }

    void push(duk_context* ctx, VirtualKey key)
    {
        duk_push_int(ctx, static_cast<int>(key));
    }
}

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
    return [ ctx, ptr = duk_get_heapptr(ctx, -1) ](
        Animation*, AnimationEvent event)
    {
        duk_push_heapptr(ctx, ptr);
        // => [ animation ]
        duk::get_prop_literal(ctx, -1, DUKR_HIDDEN_SYMBOL_CALLBACK);
        // => [ animation callback ]
        duk_dup(ctx, -2);
        // => [ animation callback animation ]
        duk::push(ctx, to_integral_value(event));
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
    ScopedStack stack{ctx};

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
    ScopedStack stack{ctx};

    duk::get_prop_literal(ctx, idx, "r");
    duk::get_prop_literal(ctx, idx, "g");
    duk::get_prop_literal(ctx, idx, "b");
    duk::get_prop_literal(ctx, idx, "a");

    return {static_cast<uint8_t>(duk_require_int(ctx, -4)),
            static_cast<uint8_t>(duk_require_int(ctx, -3)),
            static_cast<uint8_t>(duk_require_int(ctx, -2)),
            static_cast<uint8_t>(duk_require_int(ctx, -1))};
}

template <>
auto rainbow::duk::get<SharedPtr<FontAtlas>>(duk_context* ctx, duk_idx_t idx)
    -> SharedPtr<FontAtlas>
{
    return get_shared<FontAtlas>(ctx, idx);
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
    ScopedStack stack{ctx};

    duk::get_prop_literal(ctx, idx, "x");
    duk::get_prop_literal(ctx, idx, "y");

    return {static_cast<float>(duk_require_number(ctx, -2)),
            static_cast<float>(duk_require_number(ctx, -1))};
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
    push(ctx, ref.batch());
    duk::put_prop_literal(ctx, obj_idx, DUKR_HIDDEN_SYMBOL_SPRITEBATCH);
    push(ctx, ref.index());
    duk::put_prop_literal(ctx, obj_idx, DUKR_HIDDEN_SYMBOL_INDEX);

    ScopedStack stack{ctx};

    duk_push_global_object(ctx);
    duk::get_prop_literal(ctx, -1, "Rainbow");
    duk::get_prop_literal(ctx, -1, "Sprite");
    duk::get_prop_literal(ctx, -1, "prototype");

    duk_set_prototype(ctx, obj_idx);
    duk_freeze(ctx, obj_idx);
}

void rainbow::duk::push(duk_context* ctx, const Vec2f& v)
{
    PUSH_OBJECT();
    PUT_PROP(v, x);
    PUT_PROP(v, y);
}

void rainbow::duk::push(duk_context* ctx, audio::Channel* channel)
{
    PUSH_TAGGED_POINTER(ctx, channel, "Rainbow.Channel");
}

void rainbow::duk::push(duk_context* ctx, audio::Sound* sound)
{
    PUSH_TAGGED_POINTER(ctx, sound, "Rainbow.Sound");
}
