/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Definitions.h"
#if !defined(RAINBOW_JS) && defined(RAINBOW_SDL)

#include <cstring>

#ifdef RAINBOW_MAC  // Seriously, Xcode?
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#include <vorbis/vorbisfile.h>
#ifdef RAINBOW_MAC
#	pragma GCC diagnostic pop
#endif

#include "Common/Debug.h"
#include "Common/IO.h"
#include "ConFuoco/Decoder.h"

namespace
{
	const size_t kAudioBufferSize = 2048;
}

namespace ConFuoco
{
	namespace Decoder
	{
		namespace Vorbis
		{
			void error(const int err)
			{
				R_ERROR("[Rainbow::ConFuoco/Vorbis] ");
				switch (err)
				{
					case OV_EREAD:
						R_ERROR("A read from media returned an error\n");
						break;
					case OV_EFAULT:
						R_ERROR("Internal logic fault; indicates a bug or heap/stack corruption\n");
						break;
					case OV_EIMPL:
						break;
					case OV_EINVAL:
						break;
					case OV_ENOTVORBIS:
						R_ERROR("Bitstream does not contain any Vorbis data\n");
						break;
					case OV_EBADHEADER:
						R_ERROR("Invalid Vorbis bitstream header\n");
						break;
					case OV_EVERSION:
						R_ERROR("Vorbis version mismatch\n");
						break;
					case OV_ENOTAUDIO:
						break;
					case OV_EBADPACKET:
						break;
					case OV_EBADLINK:
						break;
					case OV_ENOSEEK:
						break;
					default:
						R_ERROR("Undocumented error\n");
						break;
				}
			}

			size_t fallback(char **buffer, int &channels, int &rate)
			{
				R_DEBUG("[Rainbow::ConFuoco/Vorbis] Loading fallback\n");
				*buffer = new char[22050];
				memset(*buffer, 0, 22050);
				channels = 1;
				rate = 11025;
				return 22050;
			}

			OggVorbis_File* open(const char *const file, int &channels, int &rate)
			{
				Rainbow::IO::FileHandle vorb = Rainbow::IO::open(file, Rainbow::IO::ASSET);
				if (!vorb)
				{
					R_ERROR("[Rainbow::ConFuoco/Vorbis] Failed to open '%s'\n", file);
					return nullptr;
				}

				OggVorbis_File *vf = new OggVorbis_File();
				int code = ov_open_callbacks(vorb, vf, 0, 0, OV_CALLBACKS_DEFAULT);
				if (code < 0)
				{
					delete vf;
					Rainbow::IO::close(vorb);
					R_ERROR("[Rainbow::ConFuoco/Vorbis] Failed to open '%s'\n", file);
					Vorbis::error(code);
					return nullptr;
				}

				vorbis_info *vi = ov_info(vf, -1);
				if (!vi)
				{
					delete vf;
					Rainbow::IO::close(vorb);
					R_ERROR("[Rainbow::ConFuoco/Vorbis] Failed to retrieve Ogg bitstream info\n");
					return nullptr;
				}
				channels = vi->channels;
				rate = vi->rate;
				ov_raw_seek(vf, 0);
				return vf;
			}
		}

		void close(void *handle)
		{
			if (!handle)
				return;

			OggVorbis_File *vf = static_cast<OggVorbis_File*>(handle);
			ov_clear(vf);
			delete vf;
		}

		size_t open(char **buffer, int &channels, int &rate, const char *const file)
		{
			if (!file)
				return Vorbis::fallback(buffer, channels, rate);

			OggVorbis_File *vf = Vorbis::open(file, channels, rate);
			if (!vf)
				return Vorbis::fallback(buffer, channels, rate);

			const size_t size = static_cast<size_t>(ov_pcm_total(vf, -1)) * channels * 2;
			*buffer = new char[size];
			read(*buffer, vf, size);

			ov_clear(vf);
			delete vf;
			return size;
		}

		size_t open(void **handle, char **buffer, int &channels, int &rate, const char *const file)
		{
			OggVorbis_File *vf = Vorbis::open(file, channels, rate);
			if (!vf)
				return 0;
			*handle = vf;

			const size_t size = kAudioBufferSize * channels * 2;
			*buffer = new char[size];
			return size;
		}

		size_t read(char *dst, void *src, const size_t size)
		{
			int bitstream = 0;
			int read = 0;
			size_t offset = 0;
			OggVorbis_File *vf = static_cast<OggVorbis_File*>(src);

			// Read until buffer is full
			while (offset < size)
			{
				read = ov_read(vf, dst + offset, size - offset, 0, 2, 1, &bitstream);
				if (read <= 0)
					break;
				offset += read;
			}

			memset(dst + offset, 0, size - offset);
			return offset;
		}

		void rewind(void *handle)
		{
			int code = ov_raw_seek(static_cast<OggVorbis_File*>(handle), 0);
			if (code < 0)
				Vorbis::error(code);
		}
	}
}

#endif
