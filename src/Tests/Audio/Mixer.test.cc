// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Audio/Mixer.h"

using rainbow::audio::Channel;
using rainbow::audio::Mixer;
using rainbow::audio::Sound;

namespace
{
    constexpr int kMaxAudioChannels = 8;
    constexpr const char kAudioTestFile[] = "Silence.ogg";

    struct Static
    {
        Sound* load() const
        {
            return rainbow::audio::load_sound(kAudioTestFile);
        }
    };

    struct Stream
    {
        Sound* load() const
        {
            return rainbow::audio::load_stream(kAudioTestFile);
        }
    };

    template <typename T>
    class AudioTest : public testing::Test
    {
    public:
        AudioTest() : sound_(nullptr) {}

        void SetUp() override
        {
            ASSERT_TRUE(mixer_.initialize(kMaxAudioChannels));
            sound_ = sound_type_.load();
            ASSERT_NE(nullptr, sound_);
        }

    protected:
        Mixer mixer_;
        Sound* sound_;
        T sound_type_;
    };
}

using AudioSoundTypes = ::testing::Types<Static, Stream>;
TYPED_TEST_CASE(AudioTest, AudioSoundTypes);

TYPED_TEST(AudioTest, ControlsChannelPlayback)
{
    auto channel = rainbow::audio::play(this->sound_);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    ASSERT_EQ(channel, rainbow::audio::play(channel));

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::pause(channel);

    ASSERT_TRUE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::pause(channel);

    ASSERT_TRUE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::play(channel);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::stop(channel);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_FALSE(rainbow::audio::is_playing(channel));

    rainbow::audio::play(channel);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_FALSE(rainbow::audio::is_playing(channel));

    rainbow::audio::pause(channel);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_FALSE(rainbow::audio::is_playing(channel));
}

#ifdef RAINBOW_AUDIO_AL
TYPED_TEST(AudioTest, ReusesChannels)
{
    auto channel = rainbow::audio::play(this->sound_);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::stop(channel);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_FALSE(rainbow::audio::is_playing(channel));

    ASSERT_EQ(channel, rainbow::audio::play(this->sound_));

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::stop(channel);
}

TYPED_TEST(AudioTest, ReusesSounds)
{
    ASSERT_EQ(this->sound_, this->sound_type_.load());
    rainbow::audio::release(this->sound_);
}
#endif  // RAINBOW_AUDIO_AL

TYPED_TEST(AudioTest, StopsPlaybackWhenSoundIsReleased)
{
    auto channel = rainbow::audio::play(this->sound_);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_TRUE(rainbow::audio::is_playing(channel));

    rainbow::audio::release(this->sound_);

    ASSERT_FALSE(rainbow::audio::is_paused(channel));
    ASSERT_FALSE(rainbow::audio::is_playing(channel));
}

TEST(AudioTest, CanPlaySingleSoundOnMultipleChannels)
{
    Mixer mixer_;
    ASSERT_TRUE(mixer_.initialize(kMaxAudioChannels));
    auto sound = rainbow::audio::load_sound(kAudioTestFile);

    Channel* channels[kMaxAudioChannels];
    for (auto&& channel : channels)
        channel = rainbow::audio::play(sound);

    for (int i = 0; i < kMaxAudioChannels; ++i)
    {
        for (int j = i + 1; j < kMaxAudioChannels; ++j)
            ASSERT_NE(channels[i], channels[j]);
    }

    for (int i = 0; i < kMaxAudioChannels; i += 2)
        rainbow::audio::pause(channels[i]);

    for (int i = 0; i < kMaxAudioChannels; ++i)
    {
        if (i % 2 == 0)
            ASSERT_TRUE(rainbow::audio::is_paused(channels[i]));
        else
            ASSERT_TRUE(rainbow::audio::is_playing(channels[i]));
    }

    for (int i = 1; i < kMaxAudioChannels; i += 2)
        rainbow::audio::stop(channels[i]);

    for (int i = 0; i < kMaxAudioChannels; ++i)
    {
        if (i % 2 == 0)
            ASSERT_TRUE(rainbow::audio::is_paused(channels[i]));
        else
        {
            ASSERT_FALSE(rainbow::audio::is_paused(channels[i]));
            ASSERT_FALSE(rainbow::audio::is_playing(channels[i]));
        }
    }

    for (int i = 0; i < kMaxAudioChannels; ++i)
    {
        auto ch = rainbow::audio::play(channels[i]);
        if (i % 2 == 0)
            ASSERT_EQ(channels[i], ch);
        else
            ASSERT_EQ(nullptr, ch);
    }

    for (int i = 0; i < kMaxAudioChannels; ++i)
    {
        if (i % 2 == 0)
            ASSERT_TRUE(rainbow::audio::is_playing(channels[i]));
        else
        {
            ASSERT_FALSE(rainbow::audio::is_paused(channels[i]));
            ASSERT_FALSE(rainbow::audio::is_playing(channels[i]));
        }
    }

    rainbow::audio::release(sound);

    for (auto&& channel : channels)
    {
        ASSERT_FALSE(rainbow::audio::is_paused(channel));
        ASSERT_FALSE(rainbow::audio::is_playing(channel));
    }
}
