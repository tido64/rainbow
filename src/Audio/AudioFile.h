// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AUDIOFILE_H_
#define AUDIO_AUDIOFILE_H_

#include <memory>

#include "Common/NonCopyable.h"
#include "Common/String.h"

namespace rainbow::audio
{
    struct IAudioFile : private NonCopyable<IAudioFile>
    {
        static auto open(czstring path) -> std::unique_ptr<IAudioFile>;

        virtual ~IAudioFile() {}

        virtual auto channels() const -> int = 0;
        virtual auto rate() const -> int = 0;
        virtual auto size() const -> size_t = 0;

        virtual auto read(void* dst, size_t size) -> size_t = 0;
        void rewind() { seek(0); }
        virtual bool seek(int64_t offset) = 0;

        virtual /* explicit */ operator bool() const = 0;
    };
}  // namespace rainbow::audio

#endif
