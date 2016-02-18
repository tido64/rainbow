// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_AUDIO_UMIXER_H_
#define DOTNET_RAINBOW_INTEROP_AUDIO_UMIXER_H_

#include "Audio/Mixer.h"
#include "DotNet/Rainbow/Interop/Helper.h"

namespace rainbow
{
    struct UMixer
    {
        using Channel = audio::Channel;
        using Sound = audio::Sound;

        uint32_t size_of_struct = sizeof(UMixer);
        /*StdCall*/ void (*dtor_fn)(void*) = &noop;
        /*StdCall*/ Sound* (*load_sound_fn)(const char*) = &audio::load_sound;
        /*StdCall*/ Sound* (*load_stream_fn)(const char*) = &audio::load_stream;
        /*StdCall*/ bool (*is_paused_fn)(Channel*) = &audio::is_paused;
        /*StdCall*/ bool (*is_playing_fn)(Channel*) = &audio::is_playing;
        /*StdCall*/ void (*set_loop_count_fn)(Channel*, int32_t) = &audio::set_loop_count;
        /*StdCall*/ void (*set_volume_fn)(Channel*, float) = &audio::set_volume;
        /*StdCall*/ void (*set_world_position_fn)(Channel*, Vec2f) = &audio::set_world_position;
        /*StdCall*/ void (*pause_fn)(Channel*) = &audio::pause;
        /*StdCall*/ Channel* (*play_channel_fn)(Channel*) = &audio::play;
        /*StdCall*/ Channel* (*play_sound_fn)(Sound*, Vec2f) = &audio::play;
        /*StdCall*/ void (*stop_fn)(Channel*) = &audio::stop;
    };
}

#endif
