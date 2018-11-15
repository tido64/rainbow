// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include "Common/Error.h"
#include "Common/NonCopyable.h"
#include "Common/String.h"
#include "Math/Vec2.h"

namespace rainbow::audio
{
    struct Channel;
    struct Sound;

    template <typename T>
    class TMixer : private T, private NonCopyable<TMixer<T>>
    {
    public:
        auto initialize(int max_channels) -> std::error_code
        {
            return !impl().initialize(max_channels)
                       ? ErrorCode::AudioInitializationFailed
                       : ErrorCode::Success;
        }

        void clear() { impl().clear(); }
        void process() { impl().process(); }
        void suspend(bool should_suspend) { impl().suspend(should_suspend); }

    private:
        T& impl() { return *static_cast<T*>(this); }
    };

    // Sound management

    auto load_sound(czstring path) -> Sound*;
    auto load_stream(czstring path) -> Sound*;
    void release(Sound* sound);

    // Playback

    bool is_paused(Channel*);
    bool is_playing(Channel*);

    void set_loop_count(Channel*, int count);
    void set_volume(Channel*, float volume);
    void set_world_position(Channel*, Vec2f position);

    void pause(Channel*);

    auto play(Channel*) -> Channel*;
    auto play(Sound*, Vec2f world_position = Vec2f::Zero) -> Channel*;

    void stop(Channel*);
}  // namespace rainbow::audio

#include "Platform/Macros.h"
#if defined(RAINBOW_AUDIO_AL)
#    include "Audio/AL/Mixer.h"
#elif defined(RAINBOW_AUDIO_FMOD)
#    include "Audio/FMOD/Mixer.h"
#else
#    include "Audio/cubeb/Mixer.h"
#endif

#endif
