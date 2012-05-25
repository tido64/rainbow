#ifndef CONFUOCO_STREAM_AL_H_
#define CONFUOCO_STREAM_AL_H_
#define CONFUOCO_STREAM_AL_BUFFERS 3

#include "ConFuoco/impl/Wave_AL.h"

namespace ConFuoco
{
	/// Stream a wave from disk.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Stream : public AL::Wave
	{
	public:
		Stream();
		virtual ~Stream();

		virtual bool load(const char *const file);

		/// Set the number of times a sound will return to the beginning, upon
		/// reaching the end, to repeat playback.
		/// \param loops  A value of 0 means to play the sound once. Set a
		///               positive integer value to specify the number of times
		///               to return to the start and play again. Set any
		///               negative integer value to loop the sound indefinitely
		///               until you call the stop method.
		virtual void set_loops(const int loops);

		void update();

	private:
		unsigned int bid[CONFUOCO_STREAM_AL_BUFFERS];  ///< alBuffer ids.

		int format;                ///< AL_FORMAT_MONO16 or AL_FORMAT_STEREO16.
		int rate;                  ///< Sampling rate of the stream.
		int loops;                 ///< The number of times a sound will loop.

		size_t buffer_size;  ///< Size of the stream buffer.
		char *buffer;        ///< Stream buffers.
		void *handle;        ///< Codec-specific handle.

		bool preload();
		void release();
	};
}

#endif
