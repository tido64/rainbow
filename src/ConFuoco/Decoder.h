#ifndef CONFUOCO_DECODER_H_
#define CONFUOCO_DECODER_H_

#include <cstdlib>

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
	/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	namespace Decoder
	{
		/// Close stream and release any resources.
		/// \param handle  Codec-specific handle.
		void close(void *handle);

		/// Load entire audio file into buffer.
		/// \param[out] buffer     Output buffer.
		/// \param[out] channels   Number of channels in the bitstream.
		/// \param[out] rate       Sampling rate of the bitstream.
		/// \param      file       Path to the bitstream to open.
		/// \return Size of the decoded bitstream.
		size_t open(char **buffer, int &channels, int &rate, const char *const file);

		/// Stream from file.
		/// \param[out] handle     Codec-specific handle.
		/// \param[out] buffer     Output stream buffer.
		/// \param[out] channels   Number of channels in the bitstream.
		/// \param[out] rate       Sampling rate of the bitstream.
		/// \param      file       Path to the bitstream to open.
		/// \return Size of the stream buffer.
		size_t open(void **handle, char **buffer, int &channels, int &rate, const char *const file);

		/// Read (and decode) audio frames till buffer is full.
		/// \param[out] dst   Destination for decoded bitstream.
		/// \param      src   Codec-specific handle for source bitstream.
		/// \param      size  Size of destination buffer.
		/// \return Bytes written to the buffer.
		size_t read(char *dst, void *src, const size_t size);

		/// Rewind stream to the beginning.
		/// \param handle  Codec-specific handle.
		void rewind(void *handle);
	}
}

#endif
