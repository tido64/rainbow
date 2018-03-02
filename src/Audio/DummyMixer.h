// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_DUMMYMIXER_H_
#define AUDIO_DUMMYMIXER_H_

#include "Audio/Mixer.h"

namespace rainbow::audio
{
    class DummyMixer
    {
    public:
        bool initialize() { return true; }
        void process() {}
        void suspend(bool) {}
    };

    using Mixer = TMixer<DummyMixer>;
}  // namespace rainbow::audio

#endif
