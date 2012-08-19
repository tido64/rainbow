#ifndef CONFUOCO_STREAM_SL_H_
#define CONFUOCO_STREAM_SL_H_

#include <cstdio>
#include <SLES/OpenSLES.h>

#include "ConFuoco/Wave.h"

namespace ConFuoco
{
	const size_t kAudioBufferSize = 4096;
	const size_t kAudioBuffers = 4;

	/// Stream a wave from disk.
	///
	/// \see http://opensource.apple.com/source/file/file-23/file/magic/magic.mime
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Stream : public Wave
	{
	public:
		Stream();
		virtual ~Stream();

		size_t get(char *&buffer);

		inline void update();

		virtual bool load(const char *const file) override;

		virtual void set_gain(const float gain) override;
		virtual void set_loops(const int loops) override;

		virtual void pause() override;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) override;
		virtual void stop() override;

	private:
		int loops;
		size_t buffer_ptr;
		FILE *file;

		SLObjectItf player;
		SLPlayItf play_itf;
		SLVolumeItf volume_itf;
		SLBufferQueueItf buffer_queue_itf;

		char buffers[kAudioBuffers][kAudioBufferSize];

		void preload();
		void release();
	};

	void Stream::update() { }
}

#endif
