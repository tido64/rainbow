#ifndef AUDIO_STREAM_H_
#define AUDIO_STREAM_H_

#include "ConFuoco/Wave.h"

#define STREAM_BUFFERS 3

namespace Rainbow
{
	namespace ConFuoco
	{
		/// Audio stream buffer.
		///
		/// Not used on iOS. Limited to streaming background music.
		///
		/// Copyright 2011 Bifrost Games. All rights reserved.
		/// \author Tommy Nguyen
		class Stream : public Wave
		{
		public:
			virtual ~Stream() { this->release(); }

			/// Clear buffer and reset state.
			virtual void close()
			{
				this->release();
				this->loops = true;
				this->playing = false;
				this->in_use = false;
			}

		private:
			bool loops;    ///< Whether this stream should loop
			bool playing;  ///< Whether this stream is currently playing
			bool in_use;   ///< Whether this stream is populated

			unsigned int sourced;                   ///< alSource id
			unsigned int buffered[STREAM_BUFFERS];  ///< alBuffer ids

			Stream() : loops(true), playing(false), in_use(false), sourced(-1) { }

			/// Intentionally left undefined.
			Stream(const Stream &);

			/// Intentionally left undefined.
			Stream& operator=(const Stream &);

			friend class Mixer;
		};
	}
}

#endif
