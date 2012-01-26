#ifndef CONFUOCO_WAVE_AL_H_
#define CONFUOCO_WAVE_AL_H_
#define CONFUOCO_AL_STREAM_BUFFERS 3

#include "ConFuoco/Wave.h"

namespace ConFuoco
{
	namespace AL
	{
		/// Basic wave implemented with OpenAL.
		///
		/// Implements methods common to both a sound and a stream.
		///
		/// Copyright 2012 Bifrost Entertainment. All rights reserved.
		/// \author Tommy Nguyen
		class Wave : public ConFuoco::Wave
		{
		public:
			virtual ~Wave();

			virtual void set_gain(const float gain);
			virtual void set_loops(const int loops);

			virtual void pause() const;
			virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) const;
			virtual void stop() const;

		protected:
			unsigned int sid;

			Wave();
		};
	}
}

#endif
