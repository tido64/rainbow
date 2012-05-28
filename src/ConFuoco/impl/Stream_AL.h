#ifndef CONFUOCO_STREAM_AL_H_
#define CONFUOCO_STREAM_AL_H_
#define CONFUOCO_STREAM_AL_BUFFERS 3

#include "ConFuoco/Wave.h"

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

		void update();

		virtual bool load(const char *const file);

		virtual void set_gain(const float gain);
		virtual void set_loops(const int loops);

		virtual void pause() const;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) const;
		virtual void stop() const;

	private:
		unsigned int bid[CONFUOCO_STREAM_AL_BUFFERS];  ///< alBuffer ids.
		unsigned int sid;                              ///< alSource id.

		int format;          ///< AL_FORMAT_MONO16 or AL_FORMAT_STEREO16.
		int rate;            ///< Sampling rate of the stream.
		int loops;           ///< The number of times a sound will loop.

		size_t buffer_size;  ///< Size of the stream buffer.
		char *buffer;        ///< Stream buffers.
		void *handle;        ///< Codec-specific handle.

		bool preload();
		void release();
	};
}

#endif
