#ifndef CONFUOCO_STREAM_IOS_H_
#define CONFUOCO_STREAM_IOS_H_

#include <AVFoundation/AVAudioPlayer.h>

#include "ConFuoco/Wave.h"

namespace ConFuoco
{
	/// Audio stream using AVAudioPlayer.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Stream : public Wave
	{
	public:
		inline Stream();
		virtual ~Stream();

		virtual bool load(const char *const file);

		virtual void set_gain(const float gain);
		virtual void set_loops(const int loops);

		virtual void pause() const;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) const;
		virtual void stop() const;

		inline void update();

	private:
		AVAudioPlayer* player;

		inline void release();
	};

	Stream::Stream() : player(nil) { }

	void Stream::update() { }

	void Stream::release()
	{
		if (!this->player)
			return;

		this->stop();
		this->player = nil;
	}
}

#endif
