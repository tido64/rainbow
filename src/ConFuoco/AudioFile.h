#ifndef CONFUOCO_AUDIOFILE_H_
#define CONFUOCO_AUDIOFILE_H_

namespace ConFuoco
{
	namespace Codecs
	{
		static const char kCodecOggVorbis[] = "OggS";
	}

	/// Codec-independent audio file.
	///
	/// Implements a fallback solution that fills buffers with silence on a
	/// single channel at a sample rate of 11kHz.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	class AudioFile
	{
	public:
		enum Mode
		{
			kAudioFileStatic,
			kAudioFileStream
		};

		/// Create buffer fit for audio streaming.
		/// \param[out] buffer    Created buffer.
		/// \param[in]  channels  Number of channels to output.
		/// \return Size of buffer.
		static size_t CreateBuffer(char **buffer, const unsigned int channels);

		/// Opens \p file for playback.
		/// \param file  Path to audio file.
		/// \param mode  Audio playback mode.
		/// \return Audio file handler.
		static AudioFile* Open(const char *const file, const Mode mode);

		virtual ~AudioFile();

		/// Return number of channels.
		virtual int get_channels() const;

		/// Return sample rate.
		virtual int get_rate() const;

		/// Read whole file into buffer.
		/// \param[out] dst  Destination buffer.
		/// \return Size of returned buffer.
		virtual size_t read(char **dst);

		/// Read \p size bytes into \p dst.
		/// \param dst   Destination buffer.
		/// \param size  Number of bytes to read. Must be equal to or less than
		///              the size allocated for buffer.
		/// \return Number of bytes read.
		virtual size_t read(char *dst, const size_t size);

		/// Rewind audio file to start.
		virtual void rewind();
	};
}

#endif
