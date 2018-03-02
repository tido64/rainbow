// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_CODECS_OGGVORBISAUDIOFILE_H_
#define AUDIO_CODECS_OGGVORBISAUDIOFILE_H_

#include <array>

#include "Platform/Macros.h"
#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wold-style-cast"
#   pragma clang diagnostic ignored "-Wunused-variable"
#endif
#include <vorbis/vorbisfile.h>
#ifdef __clang__
#   pragma clang diagnostic pop
#endif

#include "Audio/AudioFile.h"
#include "FileSystem/File.h"

namespace rainbow::audio
{
    class OggVorbisAudioFile final : public IAudioFile
    {
    public:
        static bool signature_matches(const std::array<uint8_t, 8>& signature);

        OggVorbisAudioFile(File);
        ~OggVorbisAudioFile() override;

        auto channels() const -> int override { return vi_->channels; }
        auto rate() const -> int override { return vi_->rate; }
        auto size() const -> size_t override;

        auto read(void*, size_t) -> size_t override;
        bool seek(int64_t) override;

        // TODO: Add `explicit` when MSVC2015 is deprecated (and remove NOLINT).
        operator bool() const override { return vi_ != nullptr; }  // NOLINT

    private:
        File file_;
        OggVorbis_File vf_;
        vorbis_info* vi_;
    };
}  // namespace rainbow::audio

#endif
