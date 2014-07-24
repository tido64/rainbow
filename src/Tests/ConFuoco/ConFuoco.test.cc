// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ConFuoco/Mixer.h"

#ifndef USE_FMOD_STUDIO
#include "ConFuoco/Sound.h"

namespace
{
	using ConFuoco::Sound;

	const char silence[] = "Silence.ogg";

	Sound* create_static_sound(ConFuoco::Mixer &mixer)
	{
		Sound *sound = mixer.create_sound(silence);
		REQUIRE(sound);
		REQUIRE(sound->type == Sound::Type::Static);
		return sound;
	}

	/**
	 * TODO: Uncomment when typed tests is implemented. See TODO below.
	 *
	Sound* create_stream_sound(ConFuoco::Mixer &mixer)
	{
		Sound *sound = mixer.create_sound(silence, Sound::Type::Stream);
		REQUIRE(sound);
		REQUIRE(sound->type == Sound::Type::Stream);
		return sound;
	}
	*/
}

TEST_CASE("Mixer instance lifetime is defined", "[confuoco]")
{
	REQUIRE(ConFuoco::Mixer::Instance == nullptr);
	ConFuoco::Mixer *mixer = new ConFuoco::Mixer();
	REQUIRE(ConFuoco::Mixer::Instance != nullptr);
	delete mixer;
	REQUIRE(ConFuoco::Mixer::Instance == nullptr);
}

// TODO: The following newlines were added for Catch to create unique names.


TEST_CASE("ConFuoco loads and plays audio", "[confuoco]")
{
	ConFuoco::Mixer mixer;
	// TODO: When typed tests is implemented: ConFuoco::Sound *sound = T(mixer);
	ConFuoco::Sound *sound = create_static_sound(mixer);

	SECTION("Mixer can control playback")
	{
		ConFuoco::Mixer::Channel *ch = mixer.play(sound);
		REQUIRE_FALSE(mixer.is_paused(ch));
		REQUIRE(mixer.is_playing(ch));
		mixer.pause(ch);
		REQUIRE(mixer.is_paused(ch));
		REQUIRE_FALSE(mixer.is_playing(ch));
		mixer.pause(ch);
		REQUIRE_FALSE(mixer.is_paused(ch));
		REQUIRE(mixer.is_playing(ch));
		mixer.stop(ch);
		REQUIRE_FALSE(mixer.is_paused(ch));
		REQUIRE_FALSE(mixer.is_playing(ch));
	}

	SECTION("Mixer handles playback interruptions")
	{
		ConFuoco::Mixer::Channel *ch = mixer.play(sound);
		REQUIRE_FALSE(mixer.is_paused(ch));
		REQUIRE(mixer.is_playing(ch));
		delete sound;
		REQUIRE_FALSE(mixer.is_paused(ch));
		REQUIRE_FALSE(mixer.is_playing(ch));
	}

	SECTION("Single source can be played on multiple channels")
	{
		const size_t count = 5;
		ConFuoco::Mixer::Channel *ch[count];

		if (sound->type == ConFuoco::Sound::Type::Static)
		{
			for (size_t i = 0; i < count; ++i)
			{
				ch[i] = mixer.play(sound);
				mixer.pause(ch[i]);
			}
			for (size_t i = 0; i < count; ++i)
			{
				REQUIRE(mixer.is_paused(ch[i]));
				REQUIRE_FALSE(mixer.is_playing(ch[i]));
				for (size_t j = i + 1; j < count; ++j)
					REQUIRE(ch[i] != ch[j]);
			}
			delete sound;
			for (size_t i = 0; i < count; ++i)
			{
				REQUIRE_FALSE(mixer.is_paused(ch[i]));
				REQUIRE_FALSE(mixer.is_playing(ch[i]));
			}
		}
		else if (sound->type == ConFuoco::Sound::Type::Stream)
		{
			ch[0] = mixer.play(sound);
			mixer.pause(ch[0]);
			REQUIRE(mixer.is_paused(ch[0]));
			REQUIRE_FALSE(mixer.is_playing(ch[0]));
			for (size_t i = 1; i < count; ++i)
			{
				ch[i] = mixer.play(sound);
				mixer.pause(ch[i]);
				REQUIRE(mixer.is_paused(ch[i]));
				REQUIRE_FALSE(mixer.is_playing(ch[i]));
				REQUIRE_FALSE(mixer.is_paused(ch[i - 1]));
				REQUIRE_FALSE(mixer.is_playing(ch[i - 1]));
			}
		}
		else
			REQUIRE_FALSE("Invalid sound type");
	}
}

TEST_CASE("Mixing buffer playback and streaming", "[confuoco]")
{
	ConFuoco::Mixer mixer;
	ConFuoco::Mixer::Channel *ch[4];

	ConFuoco::Sound *stream =
	    mixer.create_sound(silence, ConFuoco::Sound::Type::Stream);
	ch[0] = mixer.play(stream);
	REQUIRE_FALSE(mixer.is_paused(ch[0]));
	REQUIRE(mixer.is_playing(ch[0]));
	mixer.stop(ch[0]);
	REQUIRE_FALSE(mixer.is_paused(ch[0]));
	REQUIRE_FALSE(mixer.is_playing(ch[0]));

	ConFuoco::Sound *sound = mixer.create_sound(silence);
	ch[1] = mixer.play(sound);
	mixer.pause(ch[1]);

	REQUIRE(ch[0] == ch[1]);
	REQUIRE(mixer.is_paused(ch[1]));
	REQUIRE_FALSE(mixer.is_playing(ch[1]));

	ch[2] = mixer.play(stream);
	mixer.pause(ch[2]);

	REQUIRE(ch[2] != ch[0]);
	REQUIRE(mixer.is_paused(ch[1]));
	REQUIRE_FALSE(mixer.is_playing(ch[1]));
	REQUIRE(mixer.is_paused(ch[2]));
	REQUIRE_FALSE(mixer.is_playing(ch[2]));

	ch[3] = mixer.play(sound);
	mixer.pause(ch[3]);
	REQUIRE(ch[3] != ch[1]);
	REQUIRE(ch[3] != ch[2]);
	REQUIRE(mixer.is_paused(ch[1]));
	REQUIRE_FALSE(mixer.is_playing(ch[1]));
	REQUIRE(mixer.is_paused(ch[3]));
	REQUIRE_FALSE(mixer.is_playing(ch[3]));
}

#endif
