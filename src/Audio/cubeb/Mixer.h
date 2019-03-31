// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_CUBEB_MIXER_H_
#define AUDIO_CUBEB_MIXER_H_

#include <mutex>
#include <string>
#include <thread>

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#    pragma GCC diagnostic ignored "-Woverflow"
#endif
#include <absl/container/flat_hash_map.h>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif
#include <cubeb/cubeb.h>

#include "Audio/AudioFile.h"
#include "Audio/Mixer.h"
#include "Memory/BoundedPool.h"

namespace rainbow::audio
{
    class CubebMixer;

    enum class ChannelState
    {
        Stopped,
        Playing,
        Paused,
    };

    struct Channel
    {
        cubeb_stream* stream;
        std::unique_ptr<IAudioFile> source;
        intptr_t source_index;
        ChannelState state;
        int loop_count;
    };

    class CubebMixer
    {
    public:
        bool initialize(int max_channels);

        void clear();
        void process();
        void suspend(bool should_suspend);

        auto create_stream(Sound*) -> Channel*;
        void release_channel(Channel&);

        void remove_path(intptr_t index);
        auto store_path(czstring path) -> intptr_t;

    protected:
        ~CubebMixer();

    private:
        std::mutex drain_lock_;
        std::vector<Channel*> drain_;
        std::thread::id thread_id_;
        BoundedPool<Channel> channels_;
        absl::flat_hash_map<intptr_t, std::string> sounds_;
        cubeb* context_ = nullptr;

        void suspend(int (*suspend)(cubeb_stream*));
    };

    using Mixer = TMixer<CubebMixer>;
}  // namespace rainbow::audio

#endif
