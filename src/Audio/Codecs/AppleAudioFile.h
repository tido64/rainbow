// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_CODECS_APPLEAUDIOFILE_H_
#define AUDIO_CODECS_APPLEAUDIOFILE_H_

#include <AudioToolbox/ExtendedAudioFile.h>

#include "Audio/AudioFile.h"

namespace rainbow { namespace audio
{
    class AppleAudioFile final : public IAudioFile
    {
    public:
        AppleAudioFile(czstring file);
        ~AppleAudioFile() override;

        // IAudioFile overrides.

        auto channels() const -> int override
        {
            return format_.mChannelsPerFrame;
        }

        auto rate() const -> int override { return format_.mSampleRate; }
        auto size() const -> size_t override;

        auto read(void*, size_t) -> size_t override;
        bool seek(int64_t) override;

        explicit operator bool() const override { return ref_ != nullptr; }

    private:
        AudioStreamBasicDescription format_;
        ExtAudioFileRef ref_;
    };
}}  // namespace rainbow::audio

#endif
