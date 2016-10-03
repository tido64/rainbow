// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/AudioFile.h"

#include <algorithm>

#include "Common/Logging.h"

#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
#   define USE_AUDIOTOOLBOX 1
#endif
#if !defined(RAINBOW_OS_IOS)
#   define USE_OGGVORBIS 1
#endif

#ifdef USE_AUDIOTOOLBOX
#   include "Audio/Codecs/AppleAudioFile.h"
#endif  // USE_AUDIOTOOLBOX

#ifdef USE_OGGVORBIS
#   include "Audio/Codecs/OggVorbisAudioFile.h"
#endif  // USE_OGGVORBIS

using rainbow::audio::IAudioFile;

namespace
{
    constexpr size_t kAudioBufferSize = 2048;
    constexpr size_t kFallbackBufferSize = 22050;

    class DummyAudioFile final : public IAudioFile
    {
    public:
        // IAudioFile implementation.

        auto channels() const -> int override { return 1; }
        auto rate() const -> int override { return kFallbackBufferSize >> 1; }

        void rewind() override {}

        // IFile implementation.

        auto size() const -> size_t override { return kAudioBufferSize; }

        auto read(void* dst, size_t size) -> size_t override
        {
            std::fill_n(static_cast<char*>(dst), size, 0);
            return size;
        }

        auto seek(long, int) -> int override { return 0; }
        auto write(const void*, size_t) -> size_t override { return 0; }
        /*explicit*/ operator bool() const override { return true; }
    };
}

std::unique_ptr<IAudioFile> IAudioFile::open(const char* path)
{
    char signature[8]{};
    File file = File::open_asset(path);
    if (file)
    {
        file.read(signature, sizeof(signature));
        file.seek(0, SEEK_SET);
    }

#ifdef USE_OGGVORBIS
    if (OggVorbisAudioFile::signature_matches(ArrayView<char>{signature}))
    {
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
