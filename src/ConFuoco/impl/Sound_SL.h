#ifndef CONFUOCO_SOUND_SL_H_
#define CONFUOCO_SOUND_SL_H_

#include <SLES/OpenSLES.h>

#include "ConFuoco/Wave.h"
#include "ConFuoco/impl/Engine_SL.h"

namespace ConFuoco
{
	/// Sound uses the same interface as Stream but can have multiple instances
	/// that can be played simultaneously.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Sound : public Wave
	{
	public:
		Sound(const unsigned int i);
		virtual ~Sound();

		/* Implement Wave. */

		virtual bool load(const char *const file) override;

		virtual void set_gain(const float gain) override;
		virtual void set_loops(const int loops) override;

		virtual void pause() override;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) override;
		virtual void stop() override;

	private:
		bool loaded;
		bool paused;
		const size_t count;
		size_t current;
		AudioPlayer *players;

		void release();
	};
}

#endif
