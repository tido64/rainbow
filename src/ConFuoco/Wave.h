#ifndef AUDIO_WAVE_H_
#define AUDIO_WAVE_H_

#include "../Platform.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		/// Basic sound wave properties.
		///
		/// Copyright 2011 Bifrost Games. All rights reserved.
		/// \author Tommy Nguyen
		class Wave
		{
		public:
			Wave();
			virtual ~Wave() { this->release(); }

			/// Clear buffer and reset state.
			virtual void release();

		protected:
			void *buffer;              ///< Wave buffer
			void *handle;              ///< Decoder handler

		private:
			unsigned int buffer_size;  ///< Size of buffer
			unsigned int buffered;     ///< alBuffer id
			int format;                ///< The format of the wave
			int frequency;             ///< The frequency (or sample rate) of the wave

			/// Intentionally left undefined.
			Wave(const Wave &);

			/// Intentionally left undefined.
			Wave& operator=(const Wave &);

			friend class Decoder;
			friend class Mixer;
		};
	}
}

#endif
