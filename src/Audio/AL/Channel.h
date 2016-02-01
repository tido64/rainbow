// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AL_CHANNEL_H_
#define AUDIO_AL_CHANNEL_H_

#include <algorithm>

#include "Audio/AL/Sound.h"

namespace rainbow { namespace audio
{
    struct Sound;

    struct Channel
    {
    public:
        static constexpr int kNumBuffers = 3;

        Channel(unsigned int id, unsigned int* buffers)
            : id_(id), sound_(nullptr)
        {
            std::copy_n(buffers, kNumBuffers, buffers_);
        }

        auto buffers() { return buffers_; }
        auto id() const { return id_; }

        auto sound() const { return sound_; }
        void set_sound(Sound* sound) { sound_ = sound; }

    private:
        const unsigned int id_;
        unsigned int buffers_[kNumBuffers]{0, 0, 0};
        Sound* sound_;
    };
}}  // rainbow::audio

#endif
