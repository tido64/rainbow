#ifndef AUDIO_WAVE_H_
#define AUDIO_WAVE_H_

namespace Rainbow
{
	namespace ConFuoco
	{
		typedef unsigned char byte;

		/// Basic sound wave properties.

		/// Copyright 2011 Bifrost Games. All rights reserved.
		/// \author Tommy Nguyen
		class Wave
		{
		public:
			Wave() : buffer(0), buffer_size(0), buffered(-1), format(0), frequency(0) { }
			virtual ~Wave() { delete[] this->buffer; }

			/// Clear buffer and reset state.
			virtual void release();

		protected:
			unsigned char *buffer;  ///< Wave buffer

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
