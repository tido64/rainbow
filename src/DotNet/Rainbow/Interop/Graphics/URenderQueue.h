// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_URENDERQUEUE_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_URENDERQUEUE_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "Graphics/RenderQueue.h"

namespace rainbow
{
    struct URenderQueue
    {
        using RenderQueue = graphics::RenderQueue;

        template <typename T>
        static auto insert(RenderQueue* queue,
                           RenderQueue::const_iterator pos,
                           T* object,
                           const char* tag) -> int32_t
        {
            const auto id = static_cast<int32_t>(queue->size());
            queue->emplace(pos, *object, tag);
            return id;
        }

        uint32_t size_of_struct = sizeof(URenderQueue);
        /*StdCall*/ void (*dtor_fn)(void*) = &noop;

        /*ThisCall*/ int32_t (*add_animation_fn)(RenderQueue*, Animation*, const char*) =
            [](RenderQueue* queue, Animation* animation, const char* tag) {
                return insert(queue, queue->cend(), animation, tag);
            };

        /*ThisCall*/ int32_t (*add_label_fn)(RenderQueue*, Label*, const char*) =
            [](RenderQueue* queue, Label* label, const char* tag) {
                return insert(queue, queue->cend(), label, tag);
            };

        /*ThisCall*/ int32_t (*add_spritebatch_fn)(RenderQueue*, SpriteBatch*, const char*) =
            [](RenderQueue* queue, SpriteBatch* sprite_batch, const char* tag) {
                return insert(queue, queue->cend(), sprite_batch, tag);
            };

        /*ThisCall*/ void (*clear_fn)(RenderQueue*) = clear<RenderQueue>;

        /*ThisCall*/ void (*disable_fn)(RenderQueue*, int32_t) =
            [](RenderQueue* queue, int32_t unit) {
                (*queue)[unit].disable();
            };

        /*ThisCall*/ void (*disable_tag_fn)(RenderQueue*, const char*) =
            [](RenderQueue* queue, const char* tag) {
                find_invoke(*queue, tag, [](auto&& unit) { unit.disable(); });
            };

        /*ThisCall*/ void (*enable_fn)(RenderQueue*, int32_t) =
            [](RenderQueue* queue, int32_t unit) {
                (*queue)[unit].enable();
            };

        /*ThisCall*/ void (*enable_tag_fn)(RenderQueue*, const char*) =
            [](RenderQueue* queue, const char* tag) {
                find_invoke(*queue, tag, [](auto&& unit) { unit.enable(); });
            };

        /*ThisCall*/ int32_t (*insert_animation_fn)(RenderQueue*, int32_t, Animation*, const char*) =
            [](RenderQueue* queue, int32_t index, Animation* animation, const char* tag) {
                return insert(queue, queue->cbegin() + index, animation, tag);
            };

        /*ThisCall*/ int32_t (*insert_label_fn)(RenderQueue*, int32_t, Label*, const char*) =
            [](RenderQueue* queue, int32_t index, Label* label, const char* tag) {
                return insert(queue, queue->cbegin() + index, label, tag);
            };

        /*ThisCall*/ int32_t (*insert_spritebatch_fn)(RenderQueue*, int32_t, SpriteBatch*, const char*) =
            [](RenderQueue* queue, int32_t index, SpriteBatch* sprite_batch, const char* tag) {
                return insert(queue, queue->cbegin() + index, sprite_batch, tag);
            };

        /*ThisCall*/ void (*remove_at_fn)(RenderQueue*, int32_t) =
            [](RenderQueue* queue, int32_t unit) {
                queue->erase(queue->cbegin() + unit);
            };

        /*ThisCall*/ void (*remove_fn)(RenderQueue*, const char*) =
            [](RenderQueue* queue, const char* tag) {
                remove(*queue, tag);
            };

        /*ThisCall*/ void (*set_tag_fn)(RenderQueue*, int32_t, const char*) =
            [](RenderQueue* queue, int32_t unit, const char* tag) {
                (*queue)[unit].set_tag(tag);
            };
    };
}

#endif
