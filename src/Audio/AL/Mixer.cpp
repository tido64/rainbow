// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/AL/Mixer.h"

#include <cstdint>

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
#   include <OpenAL/al.h>
#   include <OpenAL/alc.h>
#   ifdef RAINBOW_OS_IOS
#       import "Audio/AL/RainbowAudioSession.h"
#   endif
#else
#   include <AL/al.h>
#   include <AL/alc.h>
#endif

#include "Audio/AudioFile.h"
#include "Audio/Mixer.h"
#include "Common/Logging.h"
#include "FileSystem/Path.h"

using rainbow::audio::ALMixer;
using rainbow::audio::Channel;
using rainbow::audio::Sound;

namespace
{
    constexpr size_t kAudioBufferSize = 8192;

    ALMixer* al_mixer = nullptr;

    struct ALCContextDestroyer
    {
        void operator()(ALCcontext* ctx) const { alcDestroyContext(ctx); }
    };

    struct ALCDeviceCloser
    {
        void operator()(ALCdevice* device) const { alcCloseDevice(device); }
    };

    auto get_channel_state(const Channel& channel)
    {
        ALint state{};
        alGetSourcei(channel.id(), AL_SOURCE_STATE, &state);
        return state;
    }

    auto get_channel_type(const Channel& channel)
    {
        ALint type{};
        alGetSourcei(channel.id(), AL_SOURCE_TYPE, &type);
        return type;
    }

    bool is_available(const Channel& channel)
    {
        return get_channel_type(channel) == AL_UNDETERMINED;
    }

    bool is_fail(ALenum result) { return result != AL_NO_ERROR; }
}

bool ALMixer::initialize(int max_channels)
{
    R_ASSERT(al_mixer == nullptr, "OpenAL is already initialised");

#ifdef RAINBOW_OS_IOS
    audio_session_ = [RainbowAudioSession audioSessionWithMixer:this];
#endif

    std::unique_ptr<ALCdevice, ALCDeviceCloser> device(alcOpenDevice(nullptr));
    if (device == nullptr)
    {
        LOGE("OpenAL: Failed to open audio device (code: 0x%x)", alGetError());
        return false;
    }

    std::unique_ptr<ALCcontext, ALCContextDestroyer> context(
        alcCreateContext(device.get(), nullptr));
    if (context == nullptr)
    {
        LOGE("OpenAL: Failed to create context (code: 0x%x)", alGetError());
        return false;
    }

    alcMakeContextCurrent(context.get());

    // macOS: Clear any errors from previous sessions.
    alGetError();

    auto sources = std::make_unique<ALuint[]>(max_channels);
    alGenSources(max_channels, sources.get());
    auto result = alGetError();
    if (is_fail(result))
    {
        LOGE("OpenAL: Failed to generate sources (code: 0x%x)", result);
        return false;
    }

    const auto buffer_count = max_channels * Channel::kNumBuffers;
    auto buffers = std::make_unique<ALuint[]>(buffer_count);
    alGenBuffers(buffer_count, buffers.get());
    result = alGetError();
    if (is_fail(result))
    {
        LOGE("OpenAL: Failed to generate buffers (code: 0x%x)", result);
        alDeleteSources(max_channels, sources.get());
        return false;
    }

    channels_.reserve(max_channels);
    for (int i = 0; i < max_channels; ++i)
    {
        const int offset = i * Channel::kNumBuffers;
        channels_.emplace_back(sources[i], buffers.get() + offset);
    }

    device.release();
    context_ = context.release();
    al_mixer = this;

    return true;
}

void ALMixer::process()
{
    static char buffer[kAudioBufferSize];
    for (Channel& channel : channels_)
    {
        const auto state = get_channel_state(channel);
        const auto type = get_channel_type(channel);
        if (type != AL_STREAMING)
        {
            if (type != AL_UNDETERMINED && state == AL_STOPPED)
                stop(&channel);
            continue;
        }

        if (state == AL_PAUSED)
            continue;

        auto sound = channel.sound();
        auto stream = sound->file.get();

        ALint processed{};
        alGetSourcei(channel.id(), AL_BUFFERS_PROCESSED, &processed);
        for (ALint i = 0; i < processed; ++i)
        {
            size_t length = stream->read(buffer, sizeof(buffer));
            if (length == 0)
            {
                if (sound->loop_count == 0)
                {
                    processed = i;
                    stop(&channel);
                    break;
                }
                else if (sound->loop_count > 0)
                {
                    --sound->loop_count;
                }
                stream->rewind();
                length = stream->read(buffer, sizeof(buffer));
            }
            ALuint bid{};
            alSourceUnqueueBuffers(channel.id(), 1, &bid);
            alBufferData(bid,
                         sound->format,
                         buffer,
                         static_cast<ALsizei>(length),
                         sound->rate);
            alSourceQueueBuffers(channel.id(), 1, &bid);
        }

        if (processed > 0 && state == AL_STOPPED)
            alSourcePlay(channel.id());
    }
}

void ALMixer::suspend(bool should_suspend)
{
    if (context_ == nullptr)
        return;

    if (should_suspend)
    {
        alcSuspendContext(context_);
        alcMakeContextCurrent(nullptr);
    }
    else
    {
        alcMakeContextCurrent(context_);
        alcProcessContext(context_);
    }
}

auto ALMixer::create_sound(const char* path) -> Sound*
{
    auto i = sounds_.find(path);
    if (i == sounds_.end())
    {
        sounds_[path] = Sound{};
        i = sounds_.find(path);
        i->second.key = i->first.c_str();
    }
    return &i->second;
}

auto ALMixer::get_channel() -> Channel*
{
    for (Channel& channel : channels_)
    {
        if (is_available(channel))
            return &channel;
    }

    return nullptr;
}

void ALMixer::release(Sound* sound)
{
    for (Channel& channel : channels_)
    {
        if (channel.sound() == sound)
            stop(&channel);
    }

    sounds_.erase(sound->key);
}

ALMixer::~ALMixer()
{
    al_mixer = nullptr;

    if (context_ == nullptr)
        return;

    suspend(true);

    for (Channel& channel : channels_)
    {
        const ALuint source = channel.id();
        alDeleteSources(1, &source);
        alDeleteBuffers(Channel::kNumBuffers, channel.buffers());
    }

    std::unique_ptr<ALCdevice, ALCDeviceCloser> device(
        alcGetContextsDevice(context_));
    alcDestroyContext(context_);
}

auto rainbow::audio::load_sound(const char* path) -> Sound*
{
    auto sound = al_mixer->create_sound(path);
    if (sound == nullptr || sound->buffer > 0)
        return sound;

    R_ASSERT(!sound->stream, "Sound already opened as a stream");

    auto audio_file = IAudioFile::open(path);
    if (!*audio_file)
    {
        release(sound);
        return nullptr;
    }

    sound->format =
        audio_file->channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    sound->rate = audio_file->rate();

    const size_t size = audio_file->size();
    auto buffer = std::make_unique<char[]>(size);
    alGenBuffers(1, &sound->buffer);
    alBufferData(sound->buffer,
                 sound->format,
                 buffer.get(),
                 static_cast<ALsizei>(audio_file->read(buffer.get(), size)),
                 sound->rate);
    return sound;
}

auto rainbow::audio::load_stream(const char* path) -> Sound*
{
    auto sound = al_mixer->create_sound(path);
    if (sound == nullptr || sound->file != nullptr)
        return sound;

    R_ASSERT(sound->buffer == 0, "Sound already opened as static");

    auto audio_file = IAudioFile::open(path);
    if (!*audio_file)
    {
        release(sound);
        return nullptr;
    }

    sound->stream = true;
    sound->format =
        audio_file->channels() == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    sound->rate = audio_file->rate();
    sound->file = std::move(audio_file);
    return sound;
}

void rainbow::audio::release(Sound* sound)
{
    const auto stream = sound->stream;
    const auto buffer = sound->buffer;

    sound->file.release();
    al_mixer->release(sound);

    if (!stream)
        alDeleteBuffers(1, &buffer);
}

bool rainbow::audio::is_paused(Channel* channel)
{
    return get_channel_state(*channel) == AL_PAUSED;
}

bool rainbow::audio::is_playing(Channel* channel)
{
    const auto state = get_channel_state(*channel);
    return state == AL_PLAYING || state == AL_PAUSED;
}

void rainbow::audio::set_loop_count(Channel* channel, int count)
{
    // TODO: Doesn't actually set loop _count_.
    alSourcei(channel->id(), AL_LOOPING, count);
}

void rainbow::audio::set_volume(Channel* channel, float volume)
{
    alSourcef(channel->id(), AL_GAIN, volume);
}

void rainbow::audio::set_world_position(Channel* channel, Vec2f position)
{
    const ALfloat pos[]{position.x, position.y, 0.0f};
    alSourcefv(channel->id(), AL_POSITION, pos);
}

void rainbow::audio::pause(Channel* channel)
{
    alSourcePause(channel->id());
}

auto rainbow::audio::play(Channel* channel) -> Channel*
{
    if (get_channel_state(*channel) == AL_STOPPED)
        return nullptr;

    alSourcePlay(channel->id());
    return channel;
}

auto rainbow::audio::play(Sound* sound, Vec2f position) -> Channel*
{
    Channel* channel = al_mixer->get_channel();
    channel->set_sound(sound);

    if (sound->stream)
    {
        static char buffer[kAudioBufferSize];

        // TODO: Prevent streaming from an already streaming sound.
        sound->file->rewind();

        int i{};
        for (; i < Channel::kNumBuffers; ++i)
        {
            const size_t size = sound->file->read(buffer, sizeof(buffer));
            alBufferData(channel->buffers()[i],
                         sound->format,
                         buffer,
                         static_cast<ALsizei>(size),
                         sound->rate);
            if (size < sizeof(buffer))
                break;
        }
        alSourceQueueBuffers(channel->id(), i, channel->buffers());
    }
    else
    {
        alSourcei(channel->id(), AL_BUFFER, sound->buffer);
    }

    set_loop_count(channel, 0);
    set_volume(channel, 1.0f);

    set_world_position(channel, position);
    alSourcePlay(channel->id());
    return channel;
}

void rainbow::audio::stop(Channel* channel)
{
    alSourceStop(channel->id());
    alSourcei(channel->id(), AL_BUFFER, AL_NONE);
    for (int i = 0; i < Channel::kNumBuffers; ++i)
        alBufferData(channel->buffers()[i], AL_FORMAT_MONO16, nullptr, 0, 0);
    channel->set_sound(nullptr);
}
