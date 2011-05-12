#ifndef AUDIO_DECODER_H_
#define AUDIO_DECODER_H_

#include <cassert>

#include "Mixer.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		/// Audio file decoder.
		///
		/// On iOS, uses its internal library for decoding audio. On other
		/// platforms we use Ogg Vorbis.
		///
		/// \see https://developer.apple.com/library/ios/#documentation/MusicAudio/Conceptual/AudioQueueProgrammingGuide/AQPlayback/PlayingAudio.html#//apple_ref/doc/uid/TP40005343-CH3-SW1
		/// \see https://developer.apple.com/library/ios/#samplecode/oalTouch/Introduction/Intro.html#//apple_ref/doc/uid/DTS40007769
		/// \see https://developer.apple.com/library/ios/#documentation/AudioVideo/Conceptual/MultimediaPG/Introduction/Introduction.html
		/// \see https://developer.apple.com/library/ios/#codinghowtos/AudioAndVideo/_index.html#//apple_ref/doc/uid/TP40007426
		/// \see https://developer.apple.com/library/ios/#documentation/MusicAudio/Conceptual/AudioQueueProgrammingGuide/Introduction/Introduction.html#//apple_ref/doc/uid/TP40005343
		/// \see http://www.wooji-juice.com/blog/iphone-openal-ima4-adpcm.html
		///
		/// Copyright 2011 Bifrost Games. All rights reserved.
		/// \author Tommy Nguyen
		class Decoder
		{
		public:
			static Decoder& Instance()
			{
				static Decoder dec;
				return dec;
			}

			/// Open a sound stream from file.
			/// \param[out] wave       Wave construct
			/// \param[in]  file       Path to file to open
			/// \param[in]  streaming  Open for streaming?
			void open(Wave &wave, const char *file, bool streaming = false);

		#ifndef RAINBOW_IOS

			/// Close stream and release any buffers;
			void close(Stream &);

			/// Read (and decode) maximum number of audio frames to stream buffer.
			/// \return Bytes read. If 0, then EOF has been reached.
			unsigned int read(Wave &wave);

			/// Rewind stream to the beginning.
			void reset(Stream &);

		#endif

		private:
			Decoder();

			/// Intentionally left undefined.
			Decoder(const Decoder &);

			/// Intentionally left undefined.
			Decoder& operator=(const Decoder &);
		};
	}
}

#endif
