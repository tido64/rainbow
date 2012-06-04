#ifndef CONFUOCO_SOUND_AL_H_
#define CONFUOCO_SOUND_AL_H_

#include "ConFuoco/OpenAL.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_SDL)

#include "ConFuoco/Wave.h"

namespace ConFuoco
{
	/// Buffered wave.
	///
	/// Loads a whole wave into buffer.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Sound : public Wave
	{
	public:
		Sound(const unsigned int i);
		virtual ~Sound();

		virtual bool load(const char *const file) override;

		virtual void set_gain(const float gain) override;
		virtual void set_loops(const int loops) override;

		virtual void pause() override;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) override;
		virtual void stop() override;

	private:
		mutable bool paused;        ///< Whether all sources have been paused.
		mutable unsigned int curr;  ///< Current source.

		unsigned int bid;           ///< alBuffer id.
		unsigned int sid_count;     ///< Number of sources (for mixing).
		unsigned int *sids;         ///< alSource ids.
	};
}

#endif  // RAINBOW_IOS || RAINBOW_SDL
#endif  // CONFUOCO_SOUND_AL_H_
