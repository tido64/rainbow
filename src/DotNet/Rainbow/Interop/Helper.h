// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_HELPER_H_
#define DOTNET_RAINBOW_INTEROP_HELPER_H_

#include <climits>
#include <cstdint>

namespace rainbow
{
    template <typename T>
    using void_t = void;

    template <typename T>
    void clear(T* ptr)
    {
        ptr->clear();
    }

    template <typename T, typename... Args>
    auto create(Args... args)  // Can't use perfect forwarding
    {
        return std::make_unique<T>(std::forward<Args>(args)...).release();
    }

    template <typename T>
    auto get_angle(T* ptr)
    {
        return ptr->angle();
    }

    template <typename T>
    auto get_color(T* ptr) -> Color
    {
        return ptr->color();
    }

    template <typename T>
    auto get_height(T* ptr) -> int32_t
    {
        return ptr->height();
    }

    template <typename T>
    auto get_position(T* ptr) -> Vec2f
    {
        return ptr->position();
    }

    template <typename T>
    auto get_scale(T* ptr)
    {
        return ptr->scale();
    }

    template <typename T>
    auto get_width(T* ptr) -> int32_t
    {
        return ptr->width();
    }

    template <typename T>
    auto is_valid(T* ptr) -> bool
    {
        return ptr->is_valid();
    }

    template <typename T>
    auto is_visible(T* ptr) -> bool
    {
        return ptr->is_visible();
    }

    template <typename T>
    void move(T* ptr, Vec2f delta)
    {
        ptr->move(delta);
    }

    void noop(void*) {}

    template <typename T>
    void release(T* ptr)
    {
        delete ptr;
    }

    template <typename T, typename = void_t<SharedPtr<T>>>
    void release_shared(T* ptr)
    {
        SharedPtr<T> release(ptr);
    }

    template <typename T>
    void set_color(T* ptr, Color color)
    {
        return ptr->set_color(color);
    }

    template <typename T>
    void set_font(T* ptr, FontAtlas* atlas)
    {
        ptr->set_font(SharedPtr<FontAtlas>{atlas});
    }

    template <typename T>
    void set_normal(T* ptr, TextureAtlas* atlas)
    {
        ptr->set_normal(SharedPtr<TextureAtlas>{atlas});
    }

    template <typename T>
    void set_position(T* ptr, Vec2f position)
    {
        return ptr->set_position(position);
    }

    template <typename T>
    void set_rotation(T* ptr, float rotation)
    {
        return ptr->set_rotation(rotation);
    }

    template <typename T>
    void set_scale(T* ptr, float scale)
    {
        return ptr->set_scale(scale);
    }

    template <typename T>
    void set_text(T* ptr, const char* text)
    {
        return ptr->set_text(text);
    }

    template <typename T>
    void set_texture(T* ptr, TextureAtlas* atlas)
    {
        ptr->set_texture(SharedPtr<TextureAtlas>{atlas});
    }

    template <typename T>
    void set_visible(T* ptr, bool visible)
    {
        ptr->set_visible(visible);
    }

    template <typename T>
    auto size(T* ptr) -> uint32_t
    {
        return static_cast<uint32_t>(ptr->size());
    }

    template <typename T, typename R>
    auto subscript_op(T* ptr, uint32_t i) -> R
    {
        return static_cast<R>((*ptr)[i]);
    }
}

#endif
