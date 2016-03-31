// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include "Common/NonCopyable.h"
#include "Math/Vec2.h"

namespace rainbow { namespace audio
{
    struct Channel;
    struct Sound;

    template <typename T>
    class TMixer : private T, private NonCopyable<TMixer<T>>
    {
    public:
        bool initialize(int max_channels)
        {
            return impl().initialize(max_channels);
        }

        void process() { impl().process(); }
        void suspend(bool should_suspend) { impl().suspend(should_suspend); }

    private:
        T& impl() { return *static_cast<T*>(this); }
    };

    // Sound management

    auto load_sound(const char* path) -> Sound*;
    auto load_stream(const char* path) -> Sound*;
    void release(Sound* sound);

    // Playback

    bool is_paused(Channel* channel);
    bool is_playing(Channel* channel);

    void set_loop_count(Channel* channel, int count);
    void set_volume(Channel* channel, float volume);
    void set_world_position(Channel* channel, const Vec2f& pos);

    void pause(Channel* channel);

    auto play(Channel* channel) -> Channel*;
    auto play(Sound* sound, const Vec2f& world_position = Vec2f::Zero)
        -> Channel*;

    void stop(Channel* channel);
}}  // rainbow::audio

#include "Platform/Macros.h"
#if defined(RAINBOW_AUDIO_AL)
#   include "Audio/AL/Mixer.h"
#elif defined(RAINBOW_AUDIO_FMOD)
#   include "Audio/FMOD/Mixer.h"
#elif defined(RAINBOW_AUDIO_WWISE)
#   include "Audio/Wwise/Mixer.h"
#elif defined(RAINBOW_AUDIO_XAUDIO2)
#   include "Audio/XAudio2/Mixer.h"
#else
#   include "Audio/DummyMixer.h"
#endif

#endif
