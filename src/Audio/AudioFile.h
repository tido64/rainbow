// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AUDIOFILE_H_
#define AUDIO_AUDIOFILE_H_

#include <memory>

#include "FileSystem/File.h"

namespace rainbow { namespace audio
{
    class IAudioFile : public IFile
    {
    public:
        static std::unique_ptr<IAudioFile> open(czstring path);

        virtual auto channels() const -> int = 0;
        virtual auto rate() const -> int = 0;

        virtual void rewind() = 0;
    };
}}  // namespace rainbow::audio

#endif
