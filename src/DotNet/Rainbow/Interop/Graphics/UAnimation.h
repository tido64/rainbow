// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_UANIMATION_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_UANIMATION_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "Graphics/Animation.h"

namespace rainbow
{
    struct UAnimation
    {
        uint32_t size_of_struct = sizeof(UAnimation);

        /*StdCall*/ Animation* (*ctor_fn)(SpriteBatch*, uint32_t, const uint32_t*, uint32_t, uint32_t, int32_t) =
            [](SpriteBatch* batch, uint32_t index, const uint32_t* frames, uint32_t frame_count, uint32_t fps, int32_t delay) {
                auto animation =
                    frame_count == 0
                        ? nullptr
                        : std::make_unique<Animation>(
                              SpriteRef{*batch, index}, nullptr, fps, delay);
                animation_set_frames(animation.get(), frames, frame_count);
                return animation.release();
            };

        /*StdCall*/ void (*dtor_fn)(Animation*) = &release<Animation>;

        /*ThisCall*/ uint32_t (*current_frame_fn)(Animation*) =
            [](Animation* animation) { return animation->current_frame(); };

        /*ThisCall*/ uint32_t (*frame_rate_fn)(Animation*) =
            [](Animation* animation) { return animation->frame_rate(); };

        /*ThisCall*/ bool (*is_stopped_fn)(Animation*) =
            [](Animation* animation) { return animation->is_stopped(); };

        /*ThisCall*/ void (*set_delay_fn)(Animation*, int32_t) =
            [](Animation* animation, int32_t delay) { animation->set_delay(delay); };

        /*ThisCall*/ void (*set_event_handler_fn)(Animation*, void*) =
            [](Animation* animation, void* handler) {
                auto propagate_event = CoreScript::Get()->propagate_animation_event();
                animation->set_callback(
                    [propagate_event, handler](Animation* animation, Animation::Event event)  //
                    {
                        auto frame = animation->current_frame();
                        propagate_event(handler, static_cast<int32_t>(event), frame);
                    });
            };

        /*ThisCall*/ void (*set_frame_rate_fn)(Animation*, uint32_t) =
            [](Animation* animation, uint32_t fps) { animation->set_frame_rate(fps); };

        /*ThisCall*/ void (*set_frames_fn)(Animation*, const uint32_t*, uint32_t) =
            &animation_set_frames;

        /*ThisCall*/ void (*set_sprite_fn)(Animation*, SpriteBatch*, uint32_t) =
            [](Animation* animation, SpriteBatch* batch, uint32_t index) {
                animation->set_sprite(SpriteRef{*batch, index});
            };

        /*ThisCall*/ void (*jump_to_fn)(Animation*, uint32_t) =
            [](Animation* animation, uint32_t frame) { animation->jump_to(frame); };

        /*ThisCall*/ void (*start_fn)(Animation*) =
            [](Animation* animation) { animation->start(); };

        /*ThisCall*/ void (*stop_fn)(Animation*) =
            [](Animation* animation) { animation->stop(); };

        static void animation_set_frames(Animation* animation,
                                         const uint32_t* frames,
                                         uint32_t count)
        {
            auto copy = std::make_unique<Animation::Frame[]>(count);
            std::copy(frames, frames + count, copy.get());
            animation->set_frames(Animation::Frames{copy.release()});
        }
    };
}

#endif
