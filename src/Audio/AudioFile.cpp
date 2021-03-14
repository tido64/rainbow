// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/AudioFile.h"

#include <algorithm>
#include <array>

#include "Common/Logging.h"
#include "FileSystem/File.h"

#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
#    include "Audio/Codecs/AppleAudioFile.h"
#    define USE_AUDIOTOOLBOX 1
#endif
#if !defined(RAINBOW_OS_IOS)
#    include "Audio/Codecs/OggVorbisAudioFile.h"
#    define USE_OGGVORBIS 1
#endif

using rainbow::czstring;
using rainbow::audio::IAudioFile;

namespace
{
    class DummyAudioFile final : public IAudioFile
    {
    public:
        // IAudioFile implementation.

        auto channels() const -> int override { return 0; }
        auto rate() const -> int override { return 0; }
        auto size() const -> size_t override { return 0; }

        auto read(void* dst, size_t size) -> size_t override
        {
            std::fill_n(static_cast<uint8_t*>(dst), size, 0);
            return size;
        }

        bool seek(int64_t) override { return false; }

        explicit operator bool() const override { return false; }
    };
}  // namespace

auto IAudioFile::open(czstring path) -> std::unique_ptr<IAudioFile>
{
    std::array<uint8_t, 8> signature{};
    File file = File::open(path, FileType::Asset);
    if (file) {
        [[maybe_unused]] auto read =
            file.read(signature.data(), signature.size());
        [[maybe_unused]] auto error = file.seek(0);
    }

#ifdef USE_OGGVORBIS
    if (OggVorbisAudioFile::signature_matches(signature)) {
        return std::unique_ptr<IAudioFile>{
            std::make_unique<OggVorbisAudioFile>(std::move(file))};
    }
#endif  // USE_OGGVORBIS

#ifdef USE_AUDIOTOOLBOX
    return std::unique_ptr<IAudioFile>{std::make_unique<AppleAudioFile>(path)};
#else
    return std::unique_ptr<IAudioFile>{std::make_unique<DummyAudioFile>()};
#endif  // USE_AUDIOTOOLBOX
}
