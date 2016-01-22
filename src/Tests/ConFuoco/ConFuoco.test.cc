// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "ConFuoco/Mixer.h"

#ifndef USE_FMOD_STUDIO
#include "ConFuoco/Sound.h"

namespace
{
    const char kSilenceOgg[] = "Silence.ogg";

    struct Static
    {
        void operator()(ConFuoco::Sound*& sound, ConFuoco::Mixer& mixer) const
        {
            sound = mixer.create_sound(kSilenceOgg);
            ASSERT_NE(nullptr, sound);
            ASSERT_EQ(ConFuoco::Sound::Type::Static, sound->type);
        }
    };

    struct Stream
    {
        void operator()(ConFuoco::Sound*& sound, ConFuoco::Mixer& mixer) const
        {
            sound = mixer.create_sound(kSilenceOgg,
                                       ConFuoco::Sound::Type::Stream);

            ASSERT_NE(nullptr, sound);
            ASSERT_EQ(ConFuoco::Sound::Type::Stream, sound->type);
        }
    };

    template <class T>
    class ConFuocoTest : public testing::Test
    {
    public:
        ConFuocoTest() : sound_(nullptr)
        {
            T create_sound;
            create_sound(sound_, mixer_);
        }

    protected:
        ConFuoco::Sound* sound_;
        ConFuoco::Mixer mixer_;
    };
}

using ConFuocoSoundTypes = ::testing::Types<Static, Stream>;
TYPED_TEST_CASE(ConFuocoTest, ConFuocoSoundTypes);

TEST(ConFuocoTest, MixerLifetime)
{
    ASSERT_EQ(nullptr, ConFuoco::Mixer::Instance);

    ConFuoco::Mixer* mixer = new ConFuoco::Mixer();

    ASSERT_NE(nullptr, ConFuoco::Mixer::Instance);

    delete mixer;

    ASSERT_EQ(nullptr, ConFuoco::Mixer::Instance);
}

TYPED_TEST(ConFuocoTest, PlaybackControl)
{
    ConFuoco::Mixer::Channel* ch = this->mixer_.play(this->sound_);

    ASSERT_TRUE(!this->mixer_.is_paused(ch));
    ASSERT_TRUE(this->mixer_.is_playing(ch));

    this->mixer_.pause(ch);

    ASSERT_TRUE(this->mixer_.is_paused(ch));
    ASSERT_TRUE(!this->mixer_.is_playing(ch));

    this->mixer_.pause(ch);

    ASSERT_TRUE(!this->mixer_.is_paused(ch));
    ASSERT_TRUE(this->mixer_.is_playing(ch));

    this->mixer_.stop(ch);

    ASSERT_TRUE(!this->mixer_.is_paused(ch));
    ASSERT_TRUE(!this->mixer_.is_playing(ch));
}

TYPED_TEST(ConFuocoTest, PlaybackInterrupted)
{
    ConFuoco::Mixer::Channel* ch = this->mixer_.play(this->sound_);

    ASSERT_TRUE(!this->mixer_.is_paused(ch));
    ASSERT_TRUE(this->mixer_.is_playing(ch));

    delete this->sound_;

    ASSERT_TRUE(!this->mixer_.is_paused(ch));
    ASSERT_TRUE(!this->mixer_.is_playing(ch));
}

TYPED_TEST(ConFuocoTest, SingleSoundMultiplePlayback)
{
    const size_t count = 5;
    ConFuoco::Mixer::Channel* ch[count];

    if (this->sound_->type == ConFuoco::Sound::Type::Static)
    {
        for (size_t i = 0; i < count; ++i)
        {
            ch[i] = this->mixer_.play(this->sound_);
            this->mixer_.pause(ch[i]);
        }

        for (size_t i = 0; i < count; ++i)
        {
            ASSERT_TRUE(this->mixer_.is_paused(ch[i]));
            ASSERT_TRUE(!this->mixer_.is_playing(ch[i]));
            for (size_t j = i + 1; j < count; ++j)
                ASSERT_TRUE(ch[i] != ch[j]);
        }

        delete this->sound_;

        for (size_t i = 0; i < count; ++i)
        {
            ASSERT_TRUE(!this->mixer_.is_paused(ch[i]));
            ASSERT_TRUE(!this->mixer_.is_playing(ch[i]));
        }
    }
    else if (this->sound_->type == ConFuoco::Sound::Type::Stream)
    {
        ch[0] = this->mixer_.play(this->sound_);
        this->mixer_.pause(ch[0]);

        ASSERT_TRUE(this->mixer_.is_paused(ch[0]));
        ASSERT_TRUE(!this->mixer_.is_playing(ch[0]));

        for (size_t i = 1; i < count; ++i)
        {
            ch[i] = this->mixer_.play(this->sound_);
            this->mixer_.pause(ch[i]);

            ASSERT_TRUE(this->mixer_.is_paused(ch[i]));
            ASSERT_TRUE(!this->mixer_.is_playing(ch[i]));
            ASSERT_TRUE(!this->mixer_.is_paused(ch[i - 1]));
            ASSERT_TRUE(!this->mixer_.is_playing(ch[i - 1]));
        }
    }
    else
        FAIL() << "Invalid sound type";
}

TEST(ConFuocoTest, MixedPlayback)
{
    ConFuoco::Mixer mixer;
    ConFuoco::Mixer::Channel* ch[4];

    ConFuoco::Sound* stream =
        mixer.create_sound(kSilenceOgg, ConFuoco::Sound::Type::Static);
    ch[0] = mixer.play(stream);

    ASSERT_TRUE(!mixer.is_paused(ch[0]));
    ASSERT_TRUE(mixer.is_playing(ch[0]));

    mixer.stop(ch[0]);

    ASSERT_TRUE(!mixer.is_paused(ch[0]));
    ASSERT_TRUE(!mixer.is_playing(ch[0]));

    ConFuoco::Sound* sound = mixer.create_sound(kSilenceOgg);
    ch[1] = mixer.play(sound);
    mixer.pause(ch[1]);

    ASSERT_EQ(ch[0], ch[1]);
    ASSERT_TRUE(mixer.is_paused(ch[1]));
    ASSERT_TRUE(!mixer.is_playing(ch[1]));

    ch[2] = mixer.play(stream);
    mixer.pause(ch[2]);

    ASSERT_NE(ch[0], ch[2]);
    ASSERT_TRUE(mixer.is_paused(ch[1]));
    ASSERT_TRUE(!mixer.is_playing(ch[1]));
    ASSERT_TRUE(mixer.is_paused(ch[2]));
    ASSERT_TRUE(!mixer.is_playing(ch[2]));

    ch[3] = mixer.play(sound);
    mixer.pause(ch[3]);

    ASSERT_NE(ch[1], ch[3]);
    ASSERT_NE(ch[2], ch[3]);
    ASSERT_TRUE(mixer.is_paused(ch[1]));
    ASSERT_TRUE(!mixer.is_playing(ch[1]));
    ASSERT_TRUE(mixer.is_paused(ch[3]));
    ASSERT_TRUE(!mixer.is_playing(ch[3]));
}

#endif
