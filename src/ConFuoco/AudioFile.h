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
		inline int get_channels() const;

		/// Return sample rate.
		inline int get_rate() const;

		/// Read whole file into buffer.
		/// \param[out] dst  Destination buffer.
		/// \return Size of returned buffer.
		inline size_t read(char **dst);

		/// Read \p size bytes into \p dst.
		/// \param dst   Destination buffer.
		/// \param size  Number of bytes to read. Must be equal to or less than
		///              the size allocated for buffer.
		/// \return Number of bytes read.
		inline size_t read(char *dst, const size_t size);

		/// Rewind audio file to start.
		inline void rewind();

	protected:
		virtual int get_channels_impl() const;
		virtual int get_rate_impl() const;
		virtual size_t read_impl(char **dst);
		virtual size_t read_impl(char *dst, const size_t size);
		virtual void rewind_impl();
	};

	int AudioFile::get_channels() const
	{
		return this->get_channels_impl();
	}

	int AudioFile::get_rate() const
	{
		return this->get_rate_impl();
	}

	size_t AudioFile::read(char **dst)
	{
		return this->read_impl(dst);
	}

	size_t AudioFile::read(char *dst, const size_t size)
	{
		return this->read_impl(dst, size);
	}

	void AudioFile::rewind()
	{
		this->rewind_impl();
	}
}

#endif
