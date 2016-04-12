// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/FMOD/Mixer.h"

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wignored-qualifiers"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   pragma GCC diagnostic ignored "-Wunused-function"
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "Common/Logging.h"

using rainbow::audio::Channel;
using rainbow::audio::FMODMixer;
using rainbow::audio::Sound;

namespace
{
    FMOD::Studio::System* fmod_studio = nullptr;
    FMOD::System* fmod_system = nullptr;

    bool is_fail(FMOD_RESULT result) { return result != FMOD_OK; }

    void log_error(FMOD_RESULT result)
    {
        LOGE("FMOD Studio: %s (%d)", FMOD_ErrorString(result), result);
    }

    template <typename F>
    FMOD::Sound* create_sound(F&& create, const char* path)
    {
#ifdef RAINBOW_OS_ANDROID
        std::string uri("file:///android_asset/");
        uri += path;
        const char* asset = uri.c_str();
#else
        const Path asset(path);
        if (!asset.is_file())
        {
            R_ABORT("No such file: %s", path);
            UNREACHABLE();
            return nullptr;
        }
#endif

        FMOD::Sound* sound;
        auto result = create(asset, FMOD_DEFAULT, nullptr, &sound);
        if (is_fail(result))
        {
            log_error(result);
            return nullptr;
        }

        return sound;
    }

    FMOD::Channel* from_opaque(Channel* channel)
    {
        return static_cast<FMOD::Channel*>(static_cast<void*>(channel));
    }

    FMOD::Sound* from_opaque(Sound* sound)
    {
        return static_cast<FMOD::Sound*>(static_cast<void*>(sound));
    }

    Channel* to_opaque(FMOD::Channel* channel)
    {
        return static_cast<Channel*>(static_cast<void*>(channel));
    }

    Sound* to_opaque(FMOD::Sound* sound)
    {
        return static_cast<Sound*>(static_cast<void*>(sound));
    }
}

bool FMODMixer::initialize(int max_channels)
{
    R_ASSERT(fmod_studio == nullptr, "FMOD Studio is already initialised");
    R_ASSERT(fmod_system == nullptr, "FMOD system is already initialised");

    auto result = FMOD::Studio::System::create(&fmod_studio);
    if (is_fail(result))
    {
        log_error(result);
        return false;
    }

    result = fmod_studio->initialize(
        max_channels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (is_fail(result))
    {
        log_error(result);
        return false;
    }

    fmod_studio->getLowLevelSystem(&fmod_system);
    return true;
}

void FMODMixer::process()
{
    ASSUME(fmod_studio != nullptr);

    if (fmod_studio == nullptr)
        return;

    fmod_studio->update();
}

void FMODMixer::suspend(bool should_suspend)
{
    if (fmod_system == nullptr)
        return;

    if (should_suspend)
        fmod_system->mixerSuspend();
    else
        fmod_system->mixerResume();
}

FMODMixer::~FMODMixer()
{
    if (fmod_studio == nullptr)
        return;

    fmod_system = nullptr;
    fmod_studio->release();
    fmod_studio = nullptr;
}

auto rainbow::audio::load_sound(const char* path) -> Sound*
{
    ASSUME(fmod_system != nullptr);

    if (fmod_system == nullptr)
        return nullptr;

    auto sound = create_sound(
        [](auto&&... args) {
            return fmod_system->createSound(
                std::forward<decltype(args)>(args)...);
        },
        path);
    return to_opaque(sound);
}

auto rainbow::audio::load_stream(const char* path) -> Sound*
{
    ASSUME(fmod_system != nullptr);

    if (fmod_system == nullptr)
        return nullptr;

    auto sound = create_sound(
        [](auto&&... args) {
            return fmod_system->createStream(
                std::forward<decltype(args)>(args)...);
        },
        path);
    return to_opaque(sound);
}

void rainbow::audio::release(Sound* sound)
{
    from_opaque(sound)->release();
}

bool rainbow::audio::is_paused(Channel* channel)
{
    bool paused{};
    from_opaque(channel)->getPaused(&paused);
    return paused;
}

bool rainbow::audio::is_playing(Channel* channel)
{
    bool isPlaying{};
    from_opaque(channel)->isPlaying(&isPlaying);
    return isPlaying;
}

void rainbow::audio::set_loop_count(Channel* channel, int count)
{
    from_opaque(channel)->setLoopCount(count);
}

void rainbow::audio::set_volume(Channel* channel, float volume)
{
    from_opaque(channel)->setVolume(volume);
}

void rainbow::audio::set_world_position(Channel* channel, const Vec2f& position)
{
    FMOD_VECTOR pos{position.x, position.y, 0.0f};
    from_opaque(channel)->set3DAttributes(&pos, nullptr);
}

void rainbow::audio::pause(Channel* channel)
{
    if (is_playing(channel))
        from_opaque(channel)->setPaused(true);
}

auto rainbow::audio::play(Channel* channel) -> Channel*
{
    if (!is_paused(channel))
        return is_playing(channel) ? channel : nullptr;

    from_opaque(channel)->setPaused(false);
    return channel;
}

auto rainbow::audio::play(Sound* sound, const Vec2f& position) -> Channel*
{
    ASSUME(fmod_system != nullptr);

    if (fmod_system == nullptr)
        return nullptr;

    FMOD::Channel* channel;
    auto result =
        fmod_system->playSound(from_opaque(sound), nullptr, false, &channel);
    if (is_fail(result))
    {
        log_error(result);
        return nullptr;
    }

    set_world_position(to_opaque(channel), position);
    return to_opaque(channel);
}

void rainbow::audio::stop(Channel* channel)
{
    from_opaque(channel)->stop();
}
