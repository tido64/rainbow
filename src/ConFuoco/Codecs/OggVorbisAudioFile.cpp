// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ConFuoco/Codecs/OggVorbisAudioFile.h"
#if USE_OGGVORBISAUDIOFILE

#include <cstring>

#include "Common/Logging.h"
#include "FileSystem/File.h"

namespace ConFuoco
{
	void ov_error(const int err)
	{
		const char *error = "Undocumented error";
		switch (err)
		{
			case OV_EREAD:
				error = "A read from media returned an error";
				break;
			case OV_EFAULT:
				error = "Internal logic fault; indicates a bug or heap/stack corruption";
				break;
			case OV_ENOTVORBIS:
				error = "Bitstream does not contain any Vorbis data";
				break;
			case OV_EBADHEADER:
				error = "Invalid Vorbis bitstream header";
				break;
			case OV_EVERSION:
				error = "Vorbis version mismatch";
				break;
			case OV_EIMPL:
			case OV_EINVAL:
			case OV_ENOTAUDIO:
			case OV_EBADPACKET:
			case OV_EBADLINK:
			case OV_ENOSEEK:
			default:
				break;
		}
		LOGE("ConFuoco/Vorbis: %s", error);
	}

#ifndef RAINBOW_OS_MACOS
	AudioFile* AudioFile::Open(const char *const file, const Mode)
	{
		File f = File::open_asset(file);
		if (!f)
			LOGE("ConFuoco/Vorbis: Failed to open '%s'", file);
		return new OggVorbisAudioFile(std::move(f));
	}
#endif

	OggVorbisAudioFile::OggVorbisAudioFile(File &&file)
	    : AudioFile(std::forward<File>(file)), vi(nullptr)
	{
		int code = ov_open_callbacks(static_cast<FILE*>(this->file), &this->vf, 0, 0, OV_CALLBACKS_DEFAULT);
		if (code < 0)
		{
			ov_error(code);
			return;
		}
		this->vi = ov_info(&this->vf, -1);
		if (!this->vi)
		{
			this->vi = nullptr;
			LOGE("ConFuoco/Vorbis: Failed to retrieve Ogg bitstream info");
		}
	}

	OggVorbisAudioFile::~OggVorbisAudioFile()
	{
		if (this->vi)
			ov_clear(&this->vf);
	}

	int OggVorbisAudioFile::channels_impl() const
	{
		if (!this->vi)
			return AudioFile::channels_impl();

		return this->vi->channels;
	}

	int OggVorbisAudioFile::rate_impl() const
	{
		if (!this->vi)
			return AudioFile::rate_impl();

		return this->vi->rate;
	}

	size_t OggVorbisAudioFile::read_impl(char **dst)
	{
		if (!this->vi)
			return AudioFile::read_impl(dst);

		this->rewind();
		const size_t size = static_cast<size_t>(ov_pcm_total(&this->vf, -1))
		                  * this->vi->channels * 2;
		*dst = new char[size];
		return this->read(*dst, size);
	}

	size_t OggVorbisAudioFile::read_impl(char *dst, const size_t size)
	{
		if (!this->vi)
			return AudioFile::read_impl(dst, size);

		int bitstream = 0;
		int read = 0;
		size_t offset = 0;

		// Read until buffer is full.
		while (offset < size)
		{
			read = ov_read(&this->vf, dst + offset, size - offset, 0, 2, 1, &bitstream);
			if (read <= 0)
				break;
			offset += read;
		}

		memset(dst + offset, 0, size - offset);
		return offset;
	}

	void OggVorbisAudioFile::rewind_impl()
	{
		if (!this->vi)
			return AudioFile::rewind_impl();

		int code = ov_raw_seek(&this->vf, 0);
		if (code < 0)
			ov_error(code);
	}
}

#endif
