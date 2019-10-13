// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AVAUDIOENGINE_MIXER_H_
#define AUDIO_AVAUDIOENGINE_MIXER_H_

#include <Foundation/NSArray.h>

#include "Audio/Mixer.h"

@class AVAudioEngine;
@class RainbowAudioPlayerNode;

#ifdef RAINBOW_OS_IOS
@class RainbowAudioSession;
#endif

namespace rainbow::audio
{
    class AudioEngineMixer
    {
    public:
        bool initialize(int max_channels);

        void clear();
        void process();
        void suspend(bool should_suspend);

        auto get_free_node() -> RainbowAudioPlayerNode*;

    protected:
        ~AudioEngineMixer();

    private:
        NSArray<RainbowAudioPlayerNode*>* nodes_ = nil;
        AVAudioEngine* engine_ = nil;
#ifdef RAINBOW_OS_IOS
        RainbowAudioSession* audio_session_ = nil;
#endif
    };

    using Mixer = TMixer<AudioEngineMixer>;
}  // namespace rainbow::audio

#endif
