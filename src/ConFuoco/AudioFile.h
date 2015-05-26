// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_AUDIOFILE_H_
#define CONFUOCO_AUDIOFILE_H_
#ifndef USE_FMOD_STUDIO

#include "FileSystem/File.h"

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
	class AudioFile
	{
	public:
		enum Mode
		{
			kAudioFileStatic,
			kAudioFileStream
		};

		/// Creates buffer fit for audio streaming.
		/// \param[out] buffer    Created buffer.
		/// \param[in]  channels  Number of channels to output.
		/// \return Size of buffer.
		static size_t CreateBuffer(char **buffer, const unsigned int channels);

		/// Opens \p file for playback.
		/// \param file  Path to audio file.
		/// \param mode  Audio playback mode.
		/// \return Audio file handler.
		static AudioFile* Open(const char *const file, const Mode mode);

		virtual ~AudioFile() = default;

		/// Returns number of channels.
		inline int channels() const;

		/// Returns sample rate.
		inline int rate() const;

		/// Reads whole file into buffer.
		/// \param[out] dst  Destination buffer.
		/// \return Size of returned buffer.
		inline size_t read(char **dst);

		/// Reads \p size bytes into \p dst.
		/// \param dst   Destination buffer.
		/// \param size  Number of bytes to read. Must be equal to or less than
		///              the size allocated for buffer.
		/// \return Number of bytes read.
		inline size_t read(char *dst, const size_t size);

		/// Rewinds audio file to start.
		inline void rewind();

	protected:
		const File file;

		virtual int channels_impl() const;
		virtual int rate_impl() const;
		virtual size_t read_impl(char **dst);
		virtual size_t read_impl(char *dst, const size_t size);
		virtual void rewind_impl();

		AudioFile() = default;
		inline AudioFile(File &&file);
	};

	int AudioFile::channels() const
	{
		return this->channels_impl();
	}

	int AudioFile::rate() const
	{
		return this->rate_impl();
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

	AudioFile::AudioFile(File &&file) : file(std::move(file)) {}
}

#endif  // !USE_FMOD_STUDIO
#endif  // CONFUOCO_AUDIOFILE_H_
