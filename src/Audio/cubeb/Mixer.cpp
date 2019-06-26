// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/cubeb/Mixer.h"

#include "Common/Error.h"
#include "Common/Logging.h"

using rainbow::czstring;
using rainbow::audio::Channel;
using rainbow::audio::ChannelState;
using rainbow::audio::CubebMixer;
using rainbow::audio::Sound;

namespace
{
    constexpr uint32_t kBufferSamples = 512;

    CubebMixer* cubeb_mixer = nullptr;

    auto as_index(Sound* sound)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<intptr_t>(sound);
    }

    void reset_channel(Channel& ch)
    {
        cubeb_stream_stop(ch.stream);
        cubeb_stream_destroy(ch.stream);
        ch.stream = nullptr;
        ch.source.reset();
        ch.source_index = 0;
        ch.state = ChannelState::Stopped;
        ch.loop_count = 0;
    }

    void state_callback(cubeb_stream* /* stream */,
                        void* user_data,
                        cubeb_state state)
    {
        switch (state)
        {
            case CUBEB_STATE_DRAINED:
            case CUBEB_STATE_ERROR:
            {
                auto& channel = *static_cast<Channel*>(user_data);
                cubeb_mixer->release_channel(channel);
                break;
            }

            default:
                break;
        }
    }
}  // namespace

bool CubebMixer::initialize(int max_channels)
{
    auto context_name = std::error_code(ErrorCode::Success).category().name();
    if (cubeb_init(&context_, context_name, nullptr) != CUBEB_OK)
    {
        LOGF("Failed to initialise cubeb");
        return false;
    }

    LOGI("cubeb: Using {} backend", cubeb_get_backend_id(context_));

    drain_.reserve(max_channels);
    thread_id_ = std::this_thread::get_id();
    channels_ = BoundedPool<Channel>{
        []() -> Channel {
            return {nullptr, nullptr, 0, ChannelState::Stopped, 0};
        },
        max_channels};

    cubeb_mixer = this;
    return true;
}

void CubebMixer::clear()
{
    for_each(channels_, [this](auto&& ch) {
        if (ch.state != ChannelState::Stopped)
            release_channel(ch);
    });
    process();
    sounds_.clear();
}

void CubebMixer::process()
{
    std::lock_guard<std::mutex> guard(drain_lock_);
    if (!drain_.empty())
    {
        for (auto&& ch : drain_)
            release_channel(*ch);
        drain_.clear();
    }
}

void CubebMixer::suspend(bool should_suspend)
{
    suspend(should_suspend ? &cubeb_stream_stop : &cubeb_stream_start);
}

auto CubebMixer::create_stream(Sound* source) -> Channel*
{
    if (channels_.empty())
        return nullptr;

    const auto index = as_index(source);
    auto path = sounds_[index].c_str();
    auto audio_file = IAudioFile::open(path);
    if (!*audio_file)
        return nullptr;

    cubeb_stream_params stream_params{
        /* format */ CUBEB_SAMPLE_S16LE,
        /* rate */ static_cast<uint32_t>(audio_file->rate()),
        /* channels */ static_cast<uint32_t>(audio_file->channels()),
        /* layout */ audio_file->channels() == 1
            ? static_cast<cubeb_channel_layout>(CUBEB_LAYOUT_MONO)
            : static_cast<cubeb_channel_layout>(CUBEB_LAYOUT_STEREO),
        /* prefs */ CUBEB_STREAM_PREF_NONE,
    };
    uint32_t latency = 0;
    if (cubeb_get_min_latency(context_, &stream_params, &latency) != CUBEB_OK)
        LOGW("cubeb: Failed to get minimum latency");

    auto& channel = *channels_.next();
    channel.source = std::move(audio_file);
    channel.source_index = index;

    auto process = []([[maybe_unused]] cubeb_stream* stream,
                      void* user_data,
                      const void* /* input_buffer */,
                      void* output_buffer,
                      long num_frames) -> long {
        NOT_USED(stream);
        auto& channel = *static_cast<Channel*>(user_data);
        R_ASSERT(stream == channel.stream, "This shouldn't happen.");
        auto& source = *channel.source;
        const auto frame_size = source.channels() * sizeof(int16_t);
        const auto read = source.read(output_buffer, num_frames * frame_size);
        const auto frames = static_cast<long>(read / frame_size);
        if (frames < num_frames && channel.loop_count > 0)
        {
            --channel.loop_count;
            source.rewind();
            const auto additional_read =
                source.read(static_cast<uint8_t*>(output_buffer) + read,
                            (num_frames - frames) * frame_size);
            return static_cast<long>(frames + (additional_read / frame_size));
        }
        return frames;
    };

    const auto result = cubeb_stream_init(context_,
                                          &channel.stream,
                                          "Rainbow Audio",
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          &stream_params,
                                          std::max(kBufferSamples, latency),
                                          process,
                                          &state_callback,
                                          &channel);
    if (result != CUBEB_OK)
    {
        LOGE("Failed to initialize stream for '{}'", path);
        return nullptr;
    }
    return &channel;
}

void CubebMixer::release_channel(Channel& channel)
{

    if (std::this_thread::get_id() != thread_id_)
    {
        std::lock_guard<std::mutex> guard(drain_lock_);
        drain_.push_back(&channel);
        return;
    }

    reset_channel(channel);
    channels_.release(channel);
}

void CubebMixer::remove_path(intptr_t index)
{
    for_each(channels_, [this, index](auto&& ch) {
        if (ch.source_index == index)
            release_channel(ch);
    });
    sounds_.erase(index);
}

auto CubebMixer::store_path(czstring path) -> intptr_t
{
    static intptr_t index = 0;
    auto [i, _] = sounds_.insert_or_assign(++index, path);
    NOT_USED(_);
    return i->first;
}

CubebMixer::~CubebMixer()
{
    for_each(channels_, [](auto&& ch) {
        if (ch.state != ChannelState::Stopped)
            reset_channel(ch);
    });
    cubeb_destroy(context_);
}

void CubebMixer::suspend(int (*suspend)(cubeb_stream* stream))
{
    for_each(channels_, [suspend](auto&& ch) {
        if (ch.state == ChannelState::Playing)
            suspend(ch.stream);
    });
}

auto rainbow::audio::load_sound(czstring path) -> Sound*
{
    return load_stream(path);
}

auto rainbow::audio::load_stream(czstring path) -> Sound*
{
    auto index = cubeb_mixer->store_path(path);
    return reinterpret_cast<Sound*>(index);
}

void rainbow::audio::release(Sound* sound)
{
    cubeb_mixer->remove_path(as_index(sound));
}

bool rainbow::audio::is_paused(Channel* channel)
{
    return channel != nullptr && channel->state == ChannelState::Paused;
}

bool rainbow::audio::is_playing(Channel* channel)
{
    return channel != nullptr && (channel->state == ChannelState::Playing ||
                                  channel->state == ChannelState::Paused);
}

void rainbow::audio::set_loop_count(Channel* channel, int count)
{
    if (channel == nullptr)
        return;

    channel->loop_count = count;
}

void rainbow::audio::set_volume(Channel* channel, float volume)
{
    if (channel == nullptr)
        return;

    cubeb_stream_set_volume(channel->stream, volume);
}

void rainbow::audio::set_world_position(Channel*, Vec2f) {}

void rainbow::audio::pause(Channel* channel)
{
    if (channel == nullptr || channel->stream == nullptr ||
        channel->state == ChannelState::Paused)
    {
        return;
    }

    cubeb_stream_stop(channel->stream);
    channel->state = ChannelState::Paused;
}

auto rainbow::audio::play(Channel* channel) -> Channel*
{
    if (channel == nullptr || channel->stream == nullptr)
        return nullptr;

    if (channel->state == ChannelState::Playing)
        return channel;

    cubeb_stream_start(channel->stream);
    channel->state = ChannelState::Playing;
    return channel;
}

auto rainbow::audio::play(Sound* sound, Vec2f) -> Channel*
{
    auto channel = cubeb_mixer->create_stream(sound);
    return play(channel);
}

void rainbow::audio::stop(Channel* channel)
{
    if (channel == nullptr || channel->state == ChannelState::Stopped)
        return;

    cubeb_mixer->release_channel(*channel);
}
