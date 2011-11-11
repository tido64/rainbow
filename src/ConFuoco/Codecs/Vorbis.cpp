#include "ConFuoco/Decoder.h"

#include <cstring>
#include <vorbis/vorbisfile.h>

#define BUFFER_SIZE 4096

namespace Rainbow
{
	namespace ConFuoco
	{
		void Decoder::open(Wave &wave, const char *const file, bool streaming)
		{
			FILE *vorb = fopen(file, "rb");
			assert(vorb || !"Rainbow::ConFuoco::Decoder: Failed to open file");

			OggVorbis_File *vf = new OggVorbis_File();
			if (ov_open_callbacks(vorb, vf, 0, 0, OV_CALLBACKS_DEFAULT) < 0)
			{
				fclose(vorb);
				assert(!"Rainbow::ConFuoco::Decoder::Vorbis: Does not appear to be an Ogg bitstream");
			}

			vorbis_info *vi = ov_info(vf, -1);
			assert(vi || !"Rainbow::ConFuoco::Decoder::Vorbis: Failed to retrieve Ogg bitstream info");
			wave.format = (vi->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			wave.frequency = vi->rate;
			wave.handle = vf;

			if (streaming)
			{
				wave.buffer_size = BUFFER_SIZE * vi->channels * 2;
				wave.buffer = new char[wave.buffer_size + 1];
			}
			else
			{
				wave.buffer_size = static_cast<unsigned int>(ov_pcm_total(vf, -1)) * vi->channels * 2;
				wave.buffer = new char[wave.buffer_size + 1];
				read(wave);
				wave.handle = nullptr;

				ov_clear(vf);
				delete vf;
			}
		}

		void Decoder::close(Stream &stream)
		{
			if (stream.handle)
			{
				OggVorbis_File *vf = static_cast<OggVorbis_File *>(stream.handle);
				ov_clear(vf);
				delete vf;
			}
			stream.close();
		}

		unsigned int Decoder::read(Wave &wave)
		{
			unsigned int offset = 0;
			int bitstream = 0;
			int read = 0;

			OggVorbis_File *vf = static_cast<OggVorbis_File *>(wave.handle);
			char *buffer = static_cast<char *>(wave.buffer);
			memset(buffer, 0, wave.buffer_size);

			while (offset < wave.buffer_size && (read = ov_read(vf, buffer + offset, wave.buffer_size - offset, 0, 2, 1, &bitstream)) != 0)
			{
				assert(read >= 0 || !"Rainbow::ConFuoco::Decoder::Vorbis: Failed to read stream");
				offset += read;
			}

			return read;
		}

		void Decoder::reset(Stream &stream)
		{
		#ifdef NDEBUG

			ov_raw_seek(static_cast<OggVorbis_File *>(stream.handle), 0);

		#else

			int failed = ov_raw_seek(static_cast<OggVorbis_File *>(stream.handle), 0);
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
				assert(!"Rainbow::ConFuoco::Decoder::Vorbis: Failed to reset stream");
			}

		#endif
		}
	}
}
