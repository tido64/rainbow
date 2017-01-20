// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AL_CHANNEL_H_
#define AUDIO_AL_CHANNEL_H_

#include <algorithm>
#include <array>

#include "Audio/AL/Sound.h"

namespace rainbow { namespace audio
{
    struct Sound;

    struct Channel
    {
    public:
        static constexpr int kNumBuffers = 3;

        Channel(uint32_t id, uint32_t* buffers)
            : id_(id), buffers_({}), sound_(nullptr)
        {
            std::copy_n(buffers, kNumBuffers, buffers_.data());
        }

        auto buffers() { return buffers_.data(); }
        auto id() const { return id_; }

        auto sound() const { return sound_; }
        void set_sound(Sound* sound) { sound_ = sound; }

    private:
        const uint32_t id_;
        std::array<uint32_t, kNumBuffers> buffers_;
        Sound* sound_;
    };
}}  // namespace rainbow::audio

#endif
