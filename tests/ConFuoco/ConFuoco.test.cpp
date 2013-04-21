#include "ConFuoco/Mixer.h"
#include "ConFuoco/Sound.h"

namespace
{
	const char silence[] = "Silence.ogg";

	struct Static
	{
		void operator()(ConFuoco::Sound *&sound, ConFuoco::Mixer &mixer) const
		{
			sound = mixer.create_sound(silence);
			ASSERT_NE(nullptr, sound);
			ASSERT_EQ(ConFuoco::STATIC, sound->type);
		}
	};

	struct Stream
	{
		void operator()(ConFuoco::Sound *&sound, ConFuoco::Mixer &mixer) const
		{
			sound = mixer.create_sound(silence, ConFuoco::STREAM);
			ASSERT_NE(nullptr, sound);
			ASSERT_EQ(ConFuoco::STREAM, sound->type);
		}
	};
}

template<class T>
class ConFuocoTest : public testing::Test
{
public:
	ConFuocoTest() : sound(nullptr)
	{
		T create_sound;
		create_sound(this->sound, this->mixer);
	}

protected:
	ConFuoco::Sound *sound;
	ConFuoco::Mixer mixer;
};

typedef ::testing::Types<Static, Stream> ConFuocoTypes;
TYPED_TEST_CASE(ConFuocoTest, ConFuocoTypes);

TEST(ConFuocoTest, MixerLifetime)
{
	ASSERT_EQ(nullptr, ConFuoco::Mixer::Instance);
	ConFuoco::Mixer *mixer = new ConFuoco::Mixer();
	ASSERT_NE(nullptr, ConFuoco::Mixer::Instance);
	delete mixer;
	ASSERT_EQ(nullptr, ConFuoco::Mixer::Instance);
}

TYPED_TEST(ConFuocoTest, PlaybackControl)
{
	ConFuoco::Channel *ch = this->mixer.play(this->sound);
	ASSERT_TRUE(!ch->is_paused() && ch->is_playing());
	ch->pause();
	ASSERT_TRUE(ch->is_paused() && ch->is_playing());
	ch->pause();
	ASSERT_TRUE(!ch->is_paused() && ch->is_playing());
	ch->stop();
	ASSERT_TRUE(!ch->is_paused() && !ch->is_playing());
}

TYPED_TEST(ConFuocoTest, PlaybackInterrupted)
{
	ConFuoco::Channel *ch = this->mixer.play(this->sound);
	ASSERT_TRUE(!ch->is_paused() && ch->is_playing());
	delete this->sound;
	ASSERT_TRUE(!ch->is_paused() && !ch->is_playing());
}

TYPED_TEST(ConFuocoTest, SingleSoundMultiplePlayback)
{
	const size_t count = 5;
	ConFuoco::Channel *ch[count];

	if (this->sound->type == ConFuoco::STATIC)
	{
		for (size_t i = 0; i < count; ++i)
		{
			ch[i] = this->mixer.play(this->sound);
			ch[i]->pause();
		}
		for (size_t i = 0; i < count; ++i)
		{
			ASSERT_TRUE(ch[i]->is_paused() && ch[i]->is_playing());
			for (size_t j = i + 1; j < count; ++j)
				ASSERT_TRUE(ch[i] != ch[j]);
		}
		delete this->sound;
		for (size_t i = 0; i < count; ++i)
			ASSERT_TRUE(!ch[i]->is_paused() && !ch[i]->is_playing());
	}
	else if (this->sound->type == ConFuoco::STREAM)
	{
		ch[0] = this->mixer.play(this->sound);
		ch[0]->pause();
		ASSERT_TRUE(ch[0]->is_paused() && ch[0]->is_playing());
		for (size_t i = 1; i < count; ++i)
		{
			ch[i] = this->mixer.play(this->sound);
			ch[i]->pause();
			ASSERT_TRUE(ch[i]->is_paused() && ch[i]->is_playing());
			ASSERT_EQ(ch[i - 1], ch[i]);
		}
	}
	else
		FAIL() << "Invalid sound type";
}

TEST(ConFuocoTest, MixedPlayback)
{
	ConFuoco::Mixer mixer;
	ConFuoco::Channel *ch[4];

	ConFuoco::Sound *stream = mixer.create_sound(silence, ConFuoco::STATIC);
	ch[0] = mixer.play(stream);
	ASSERT_TRUE(!ch[0]->is_paused() && ch[0]->is_playing());
	ch[0]->stop();
	ASSERT_TRUE(!ch[0]->is_paused() && !ch[0]->is_playing());

	ConFuoco::Sound *sound = mixer.create_sound(silence);
	ch[1] = mixer.play(sound);
	ch[1]->pause();

	ASSERT_EQ(ch[0], ch[1]);
	ASSERT_TRUE(ch[1]->is_paused() && ch[1]->is_playing());

	ch[2] = mixer.play(stream);
	ch[2]->pause();

	ASSERT_NE(ch[0], ch[2]);
	ASSERT_TRUE(ch[1]->is_paused() && ch[1]->is_playing());
	ASSERT_TRUE(ch[2]->is_paused() && ch[2]->is_playing());

	ch[3] = mixer.play(sound);
	ch[3]->pause();
	ASSERT_NE(ch[1], ch[3]);
	ASSERT_NE(ch[2], ch[3]);
	ASSERT_TRUE(ch[1]->is_paused() && ch[1]->is_playing());
	ASSERT_TRUE(ch[3]->is_paused() && ch[3]->is_playing());
}
