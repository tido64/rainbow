// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_CODECS_OGGVORBISAUDIOFILE_H_
#define AUDIO_CODECS_OGGVORBISAUDIOFILE_H_

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_MACOS
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#include <vorbis/vorbisfile.h>
#ifdef RAINBOW_OS_MACOS
#   pragma GCC diagnostic pop
#endif

#include "Audio/AudioFile.h"
#include "Memory/ArrayView.h"

namespace rainbow { namespace audio
{
    class OggVorbisAudioFile final : public IAudioFile
    {
    public:
        static bool signature_matches(const ArrayView<const char>& signature);

        OggVorbisAudioFile(File&&);
        ~OggVorbisAudioFile() override;

        // IAudioFile overrides.

        auto channels() const -> int override { return vi_->channels; }
        auto rate() const -> int override { return vi_->rate; }
        void rewind() override { seek(0, 0); }

        // IFile overrides.

        auto size() const -> size_t override;
        auto read(void*, size_t) -> size_t override;
        auto seek(long, int) -> int override;
        auto write(const void*, size_t) -> size_t override { return 0; }
        /*explicit*/ operator bool() const override { return vi_ != nullptr; }

    private:
        File file_;
        OggVorbis_File vf_;
        vorbis_info* vi_;
    };
}}  // namespace rainbow::audio

#endif
