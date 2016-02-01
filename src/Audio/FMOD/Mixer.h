// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_FMOD_MIXER_H_
#define AUDIO_FMOD_MIXER_H_

#include "Audio/Mixer.h"

namespace rainbow { namespace audio
{
    class FMODMixer
    {
    public:
        bool initialize(int max_channels);
        void process();
        void suspend(bool should_suspend);

    protected:
        ~FMODMixer();
    };

    using Mixer = TMixer<FMODMixer>;
}}  // namespace rainbow::audio

#endif
