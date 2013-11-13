/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID

#include "Common/IO.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Sound.h"
#include "ConFuoco/impl/Recorder_SL.h"

#define slCreateAudioPlayer(self, player, source, sink, interfaces, iids, req) \
	(*self)->CreateAudioPlayer(self, player, source, sink, interfaces, iids, req)
#define slCreateAudioRecorder(self, recorder, source, sink, interfaces, iids, req) \
	(*self)->CreateAudioRecorder(self, recorder, source, sink, interfaces, iids, req)
#define slCreateOutputMix(self, mix, interfaces, iids, req) \
	(*self)->CreateOutputMix(self, mix, interfaces, iids, req)
#define slDestroy(self) \
	(*self)->Destroy(self)
#define slGetInterface(self, iid, interface) \
	(*self)->GetInterface(self, iid, interface)
#define slGetPlayState(self, state) \
	(*self)->GetPlayState(self, state);
#define slRealize(self, async) \
	(*self)->Realize(self, async)
#define slRegisterCallback(self, callback, context) \
	(*self)->RegisterCallback(self, callback, context)
#define slSetCallbackEventsMask(self, flags) \
	(*self)->SetCallbackEventsMask(self, flags)
#define slSetPlayState(self, state) \
	(*self)->SetPlayState(self, state);
#define slSetVolumeLevel(self, level) \
	(*self)->SetVolumeLevel(self, level)

namespace ConFuoco
{
	namespace
	{
	#if 0  // Only used when creating an audio player with SLDataLocator_URI
		SLDataFormat_MIME GetMIME(const short *const header)
		{
			SLDataFormat_MIME format = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
			if ((header[0] & 0xfffe) == 0xfffa || memcmp(header, "ID3", 3) == 0)
			{
				format.mimeType = (SLchar*)"audio/mpeg";
				format.containerType = SL_CONTAINERTYPE_MP3;
			}
			else if (memcmp(header, "RIFF", 4) == 0)
			{
				format.mimeType = (SLchar*)"audio/x-wav";
				format.containerType = SL_CONTAINERTYPE_WAV;
			}
			else if ((header[0] & 0xffe0) == 0x56e0 || header[0] == 0x4de1)
			{
				format.mimeType = (SLchar*)"audio/MP4A-LATM";
				format.containerType = SL_CONTAINERTYPE_AAC;
			}
			else if (memcmp(header, "OggS", 4) == 0)
			{
				format.mimeType = (SLchar*)"application/ogg";
				format.containerType = SL_CONTAINERTYPE_OGG;
			}
			return format;
		}
	#endif

		/// Support looping a given number of times. The SL_IID_SEEK interface
		/// currently does not support this.
		void PlayCallback(SLPlayItf caller, void *pContext, SLuint32)
		{
			int &loops = *static_cast<int*>(pContext);
			if (loops == 0)
				return;
			if (loops > 0)
				--loops;
			slSetPlayState(caller, SL_PLAYSTATE_STOPPED);
			slSetPlayState(caller, SL_PLAYSTATE_PLAYING);
		}

		struct SLContext
		{
			SLObjectItf engine_obj;  ///< OpenSL engine object.
			SLEngineItf engine;      ///< OpenSL engine interface.
			SLObjectItf mix_obj;     ///< Output mix interface.

			SLContext()
			{
				if (slCreateEngine(&this->engine_obj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to create SL engine\n");
					return;
				}
				if (slRealize(this->engine_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to realize SL engine\n");
					return;
				}
				if (slGetInterface(this->engine_obj, SL_IID_ENGINE, &this->engine) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to get interface for SL engine\n");
					return;
				}
				const SLInterfaceID iids[] = { SL_IID_ENVIRONMENTALREVERB };
				const SLboolean req[] = { SL_BOOLEAN_FALSE };
				if (slCreateOutputMix(this->engine, &this->mix_obj, 1, iids, req) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to create SL output mix\n");
					return;
				}
				if (slRealize(this->mix_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to realize SL output mix\n");
					return;
				}
			}

			~SLContext()
			{
				if (this->mix_obj)
					slDestroy(this->mix_obj);
				if (this->engine_obj)
					slDestroy(this->engine_obj);
			}
		};

		struct Stream : public Sound
		{
			unsigned int channel;
			int loops;
			SLDataLocator_AndroidFD fd;

			Stream(const int type, Mixer *m, SLDataLocator_AndroidFD &fd, int loops = 0) :
				Sound(type, m), channel(Mixer::num_channels), loops(loops), fd(fd) { }

			virtual ~Stream()
			{
				this->mixer->release(this);
			}
		};

		struct StreamPlayer
		{
			bool focus;
			bool paused;
			int loops;
			Stream *stream;
			SLObjectItf player;
			SLPlayItf play_itf;
			SLVolumeItf volume_itf;

			StreamPlayer() :
				focus(true), paused(false), loops(-1), stream(nullptr),
				player(nullptr), play_itf(nullptr), volume_itf(nullptr) { }

			bool load(SLContext *context, Stream *stream)
			{
				const SLInterfaceID iids[] = { SL_IID_VOLUME };
				const SLboolean req[] = { SL_BOOLEAN_TRUE };

				this->release();

				SLDataFormat_MIME format = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
				SLDataSource source = { &stream->fd, &format };

				SLDataLocator_OutputMix output_mix = { SL_DATALOCATOR_OUTPUTMIX, context->mix_obj };
				SLDataSink sink = { &output_mix, nullptr };

				if (slCreateAudioPlayer(context->engine, &this->player, &source, &sink, 1, iids, req) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to create audio player (%p)\n", stream);
					this->release();
					return false;
				}
				if (slRealize(this->player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to realize audio player (%p)\n", stream);
					this->release();
					return false;
				}
				if (slGetInterface(this->player, SL_IID_PLAY, &this->play_itf) != SL_RESULT_SUCCESS)
					R_ERROR("[Rainbow::ConFuoco] Failed to get playback interface (%p)\n", stream);
				if (!this->play_itf)
				{
					this->release();
					return false;
				}
				if (stream->type == STREAM)
				{
					this->loops = stream->loops;
					if (slSetCallbackEventsMask(this->play_itf, SL_PLAYEVENT_HEADATEND) != SL_RESULT_SUCCESS
					    || slRegisterCallback(this->play_itf, PlayCallback, &this->loops) != SL_RESULT_SUCCESS)
					{
						this->release();
						return false;
					}
				}
				if (slGetInterface(this->player, SL_IID_VOLUME, &this->volume_itf) != SL_RESULT_SUCCESS)
					R_ERROR("[Rainbow::ConFuoco] Failed to get volume interface (%p)\n", stream);
				return true;
			}

			void release()
			{
				this->loops = -1;
				this->stream = nullptr;
				if (this->player)
				{
					slDestroy(this->player);
					this->player = nullptr;
					this->play_itf = nullptr;
					this->volume_itf = nullptr;
				}
			}

			~StreamPlayer()
			{
				this->release();
			}
		};

		StreamPlayer players[Mixer::num_channels];
	}

	Mixer *Mixer::Instance = nullptr;

	Mixer::Mixer() : master_gain(1.0f), context(new SLContext())
	{
		R_ASSERT(!Instance, "An instance of ConFuoco::Mixer already exists");

		for (size_t i = 0; i < num_channels; ++i)
		{
			this->channels[i].ch = i;
			this->channels[i].mixer = this;
		}
		Instance = this;
	}

	Mixer::~Mixer()
	{
		Instance = nullptr;
		this->clear();
		delete static_cast<SLContext*>(this->context);
	}

	void Mixer::set_gain(const float gain)
	{
		this->master_gain = gain;
	}

	void Mixer::set_pitch(const float) { }

	void Mixer::clear()
	{
		for (size_t i = 0; i < num_channels; ++i)
		{
			this->channels[i].sound = nullptr;
			players[i].release();
		}
	}

	void* Mixer::create_recorder()
	{
		return new SLRecorder(static_cast<SLContext*>(this->context)->engine);
	}

	Sound* Mixer::create_sound(const char *const file, const int type, const int loops)
	{
		#if 0
			/**
			 * Disabled because creating an audio player with SLDataLocator_URI
			 * doesn't seem to work. Android implements OpenSL 1.0.1 *partially*
			 * and has _extensions_ instead.
			 */
			char asset_uri[256] = "file:///android_asset/";
			strncpy(asset_uri, file, 232);
			SLDataLocator_URI uri = { SL_DATALOCATOR_URI, reinterpret_cast<SLchar*>(asset_uri) };
			SLDataFormat_MIME format = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
			SLDataSource source = { &uri, &format };
		#endif

		Rainbow::IO::FileHandle asset =
				Rainbow::IO::find_and_open(file, Rainbow::IO::kIOTypeAsset);
		if (!asset)
		{
			R_DEBUG("[Rainbow::ConFuoco/SL] Failed to open '%s'\n", file);
			return nullptr;
		}

		off_t offset, length;
		int fd = AAsset_openFileDescriptor(asset, &offset, &length);
		Rainbow::IO::close(asset);

		SLDataLocator_AndroidFD android_fd = { SL_DATALOCATOR_ANDROIDFD, fd, offset, length };

		Stream *stream = new Stream(type, this, android_fd, loops);
		this->sounds.push_back(stream);
		R_DEBUG("[Rainbow::ConFuoco/SL] '%s' => %p (stream)\n", file, static_cast<void*>(stream));
		return stream;
	}

	Channel* Mixer::play(Sound *snd)
	{
		Channel *ch = nullptr;
		Stream *s = static_cast<Stream*>(snd);
		if (snd->type != STATIC
		    && s->channel < num_channels
		    && this->channels[s->channel].sound == snd)
		{
			ch = this->channels + s->channel;
			this->stop(ch);
		}
		else
		{
			ch = this->next_channel();
			if (!ch)
			{
				R_DEBUG("[Rainbow::ConFuoco/SL] All channels are busy\n");
				return nullptr;
			}

			s->channel = ch->ch;
			bool success = players[s->channel].load(static_cast<SLContext*>(this->context), s);
			if (!success)
			{
				R_DEBUG("[Rainbow::ConFuoco/SL] Failed to load %p\n", static_cast<void*>(snd));
				return nullptr;
			}
		}
		ch->sound = snd;
		this->set_gain(ch, this->master_gain);
		this->play(ch);
		R_DEBUG("[Rainbow::ConFuoco/SL] %p -> %u\n", static_cast<void*>(snd), ch->ch);
		return ch;
	}

	void Mixer::release(Sound *snd)
	{
		Channel *ch = this->channels;
		for (size_t i = 0; i < num_channels; ++i, ++ch)
		{
			if (ch->sound == snd)
			{
				this->stop(ch);
				ch->sound = nullptr;
				players[i].release();
			}
		}
		R_DEBUG("[Rainbow::ConFuoco/SL] <- %p\n", static_cast<void*>(snd));
	}

	void Mixer::suspend(const bool suspend)
	{
		if (suspend)
		{
			SLuint32 state;
			for (size_t i = 0; i < num_channels; ++i)
			{
				StreamPlayer &player = players[i];
				if (!player.play_itf)
					continue;
				slGetPlayState(player.play_itf, &state);
				if (state == SL_PLAYSTATE_PLAYING)
				{
					this->pause(this->channels + i);
					player.focus = false;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < num_channels; ++i)
			{
				StreamPlayer &player = players[i];
				if (!player.focus)
				{
					player.focus = true;
					this->play(this->channels + i);
				}
			}
		}
	}

	void Mixer::update() { }

	bool Mixer::is_paused(Channel *ch)
	{
		return players[ch->ch].paused;
	}

	bool Mixer::is_playing(Channel *ch)
	{
		SLuint32 state;
		SLPlayItf &itf = players[ch->ch].play_itf;
		slGetPlayState(itf, &state);
		return players[ch->ch].paused || state == SL_PLAYSTATE_PLAYING;
	}

	void Mixer::set_gain(Channel *ch, const float gain)
	{
		SLVolumeItf &itf = players[ch->ch].volume_itf;
		slSetVolumeLevel(itf, (1.0f - gain) * -100 * this->master_gain);
	}

	void Mixer::pause(Channel *ch)
	{
		slSetPlayState(players[ch->ch].play_itf, SL_PLAYSTATE_PAUSED);
		players[ch->ch].paused = true;
	}

	void Mixer::play(Channel *ch)
	{
		slSetPlayState(players[ch->ch].play_itf, SL_PLAYSTATE_PLAYING);
		players[ch->ch].paused = false;
	}

	void Mixer::stop(Channel *ch)
	{
		slSetPlayState(players[ch->ch].play_itf, SL_PLAYSTATE_STOPPED);
		players[ch->ch].paused = false;
	}

	SLRecorder::SLRecorder(SLEngineItf engine) :
		active_buffer(0), object(nullptr), interface(nullptr), buffer(nullptr), samples(nullptr)
	{
		SLDataLocator_IODevice iodev = {
			SL_DATALOCATOR_IODEVICE,
			SL_IODEVICE_AUDIOINPUT,
			SL_DEFAULTDEVICEID_AUDIOINPUT,
			nullptr
		};
		SLDataSource source = { &iodev, nullptr };

		SLDataLocator_AndroidSimpleBufferQueue buffer_queue = {
			SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, kNumInputSampleBuffers
		};
		SLDataFormat_PCM pcm_format = {
			SL_DATAFORMAT_PCM,
			1,
			SL_SAMPLINGRATE_16,
			SL_PCMSAMPLEFORMAT_FIXED_16,
			SL_PCMSAMPLEFORMAT_FIXED_16,
			SL_SPEAKER_FRONT_CENTER,
			SL_BYTEORDER_LITTLEENDIAN
		};
		SLDataSink sink = { &buffer_queue, &pcm_format };

		const SLInterfaceID iids[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
		const SLboolean req[] = { SL_BOOLEAN_TRUE };

		if (slCreateAudioRecorder(engine, &(this->object), &source, &sink, 1, iids, req) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to create audio recorder\n");
			return;
		}
		if (slRealize(this->object, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to realize audio recorder\n");
			return;
		}
		if (slGetInterface(this->object, SL_IID_RECORD, &this->interface) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to get audio recorder interface\n");
			return;
		}

		const size_t sz = kInputSamples * kNumInputSampleBuffers;
		this->buffer = new short[sz];
		memset(this->buffer, 0, sz);
	}

	SLRecorder::~SLRecorder()
	{
		if (this->object)
			slDestroy(this->object);
		delete this->buffer;
	}

	void SLRecorder::set_callback(slAndroidSimpleBufferQueueCallback callback, void *context)
	{
		if (slGetInterface(this->object, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &this->buffer_queue) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to get buffer queue interface\n");
			return;
		}
		if (slRegisterCallback(buffer_queue, callback, context) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to set audio recorder callback\n");
			return;
		}
	}
}

#undef slSetVolumeLevel
#undef slSetPlayState
#undef slSetCallbackEventsMask
#undef slRegisterCallback
#undef slRealize
#undef slGetPlayState
#undef slGetInterface
#undef slDestroy
#undef slCreateOutputMix
#undef slCreateAudioRecorder
#undef slCreateAudioPlayer
#endif
