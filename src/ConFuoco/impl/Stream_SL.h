#ifndef CONFUOCO_STREAM_SL_H_
#define CONFUOCO_STREAM_SL_H_

#include <SLES/OpenSLES.h>

#include "ConFuoco/Wave.h"
#include "ConFuoco/impl/Engine_SL.h"

namespace ConFuoco
{
	/// Stream a wave from disk.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Stream : public Wave
	{
	public:
		Stream();
		virtual ~Stream();

		inline void update();

		/* Implement Wave. */

		virtual bool load(const char *const file) override;

		virtual void set_gain(const float gain) override;
		virtual void set_loops(const int loops) override;

		virtual void pause() override;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) override;
		virtual void stop() override;

	private:
		int loops;
		AudioPlayer player;

		void release();
	};

	void Stream::update() { }
}

#endif
