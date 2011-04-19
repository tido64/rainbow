#ifndef AUDIO_DECODER_H_
#define AUDIO_DECODER_H_

#include <cassert>

#include "Mixer.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		/// Audio file decoder.

		/// Automatically detects audio encoding using FFmpeg on all
		/// platform except for iOS. On which the built-in engine is
		/// used instead.
		///
		/// iOS development documents:
		/// \see https://developer.apple.com/library/ios/#documentation/MusicAudio/Conceptual/AudioQueueProgrammingGuide/AQPlayback/PlayingAudio.html#//apple_ref/doc/uid/TP40005343-CH3-SW1
		/// \see https://developer.apple.com/library/ios/#samplecode/oalTouch/Introduction/Intro.html#//apple_ref/doc/uid/DTS40007769
		/// \see https://developer.apple.com/library/ios/#documentation/AudioVideo/Conceptual/MultimediaPG/Introduction/Introduction.html
		/// \see https://developer.apple.com/library/ios/#codinghowtos/AudioAndVideo/_index.html#//apple_ref/doc/uid/TP40007426
		/// \see https://developer.apple.com/library/ios/#documentation/MusicAudio/Conceptual/AudioQueueProgrammingGuide/Introduction/Introduction.html#//apple_ref/doc/uid/TP40005343
		/// \see http://www.wooji-juice.com/blog/iphone-openal-ima4-adpcm.html
		///
		/// FFmpeg documents:
		/// \see http://kcat.strangesoft.net/alffmpeg.c
		/// \see http://dranger.com/ffmpeg/ffmpeg.html
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
			void open(Wave &, const char *);

		#ifndef RAINBOW_IOS

			/// Open a sound stream from file.
			void open_stream(Stream &, const char *);

			/// Read (and decode) maximum number of audio frames to stream buffer.
			/// \return The number of decoded frames
			//unsigned int read(Stream &, AudioQueueBufferRef);
			unsigned int read(Stream &);

			/// Rewind stream.
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
