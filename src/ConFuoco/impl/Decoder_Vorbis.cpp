#include "Platform.h"
#ifdef RAINBOW_SDL

#include <cstring>

#include <vorbis/vorbisfile.h>

#include "Common/RainbowAssert.h"
#include "ConFuoco/Decoder.h"

namespace ConFuoco
{
	namespace Vorbis
	{
		static OggVorbis_File* open(const char *const file, int &channels, int &rate)
		{
			FILE *vorb = fopen(file, "rb");
			if (!vorb)
			{
				fprintf(stderr, "Rainbow::ConFuoco::Decoder::open: Failed to open file\n");
				return nullptr;
			}

			OggVorbis_File *vf = new OggVorbis_File();
			if (ov_open_callbacks(vorb, vf, 0, 0, OV_CALLBACKS_DEFAULT) < 0)
			{
				fclose(vorb);
				fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis::open: Does not appear to be an Ogg bitstream\n");
				return nullptr;
			}

			vorbis_info *vi = ov_info(vf, -1);
			if (!vi)
			{
				delete vf;
				fclose(vorb);
				fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis::open: Failed to retrieve Ogg bitstream info\n");
				return nullptr;
			}
			channels = vi->channels;
			rate = vi->rate;
			return vf;
		}
	}

	size_t Decoder::open_stream(void **handle, char **buffer, int &channels, int &rate, const char *const file)
	{
		OggVorbis_File *vf = Vorbis::open(file, channels, rate);
		if (!vf)
			return 0;
		*handle = vf;

		const size_t size = 4096 * channels * 2;
		*buffer = new char[size];
		return size;
	}

	size_t Decoder::open_wave(char **buffer, int &channels, int &rate, const char *const file)
	{
		OggVorbis_File *vf = Vorbis::open(file, channels, rate);
		if (!vf)
			return 0;

		const size_t size = static_cast<size_t>(ov_pcm_total(vf, -1)) * channels * 2;
		*buffer = new char[size];
		read(*buffer, vf, size);

		ov_clear(vf);
		delete vf;
		return size;
	}

	void Decoder::close(void *handle)
	{
		R_ASSERT(handle, "close: Missing handle.");

		OggVorbis_File *vf = static_cast<OggVorbis_File*>(handle);
		ov_clear(vf);
		delete vf;
	}

	size_t Decoder::read(char *dst, void *src, const size_t size)
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

	void Decoder::reset(void *handle)
	{
	#ifdef NDEBUG

		ov_raw_seek(static_cast<OggVorbis_File*>(handle), 0);

	#else

		int failed = ov_raw_seek(static_cast<OggVorbis_File*>(handle), 0);
		if (failed)
		{
			switch (failed)
			{
				case OV_ENOSEEK:
					fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis:OV_ENOSEEK\n");
					break;
				case OV_EINVAL:
					fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis:OV_EINVAL\n");
					break;
				case OV_EREAD:
					fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis:OV_EREAD\n");
					break;
				case OV_EFAULT:
					fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis:OV_EFAULT\n");
					break;
				case OV_EBADLINK:
					fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis:OV_EBADLINK\n");
					break;
				default:
					fprintf(stderr, "Rainbow::ConFuoco::Decoder::Vorbis:OV_EUNKNOWN\n");
					break;
			}
			R_ASSERT(false, "reset: Failed to reset stream.");
		}

	#endif
	}
}

#endif
