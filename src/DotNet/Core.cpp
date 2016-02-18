// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#define RAINBOW_EXPORT
#include <Rainbow.hpp>

void rainbow_animation_set_event_handler(Animation* animation,
                                         rbAnimationEventHandler handler,
                                         void* context)
{
    if (handler == nullptr)
    {
        animation->set_callback(nullptr);
    }
    else
    {
        animation->set_callback(  //
            [handler, context](Animation* animation, AnimationEvent ev) {
                handler(animation, ev, animation->current_frame(), context);
            });
    }
}
