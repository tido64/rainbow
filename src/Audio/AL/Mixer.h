// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AL_MIXER_H_
#define AUDIO_AL_MIXER_H_

#include <string>
#include <vector>

#include "ThirdParty/DisableWarnings.h"
#include <absl/container/node_hash_map.h>
#include "ThirdParty/ReenableWarnings.h"

#include "Audio/AL/Channel.h"
#include "Audio/AL/Sound.h"
#include "Audio/Mixer.h"

typedef struct ALCcontext_struct ALCcontext;

#ifdef RAINBOW_OS_IOS
@class RainbowAudioSession;
#endif

namespace rainbow::audio
{
    class ALMixer
    {
    public:
        bool initialize(int max_channels);

        void clear();
        void process();
        void suspend(bool should_suspend);

        auto create_sound(czstring path) -> Sound*;
        auto get_channel() -> Channel*;
        void release(Sound* sound);

    protected:
        ~ALMixer();

    private:
        std::vector<Channel> channels_;
        absl::node_hash_map<std::string, Sound> sounds_;
        ALCcontext* context_ = nullptr;
#ifdef RAINBOW_OS_IOS
        RainbowAudioSession* audio_session_ = nil;
#endif
    };

    using Mixer = TMixer<ALMixer>;
}  // namespace rainbow::audio

#endif
