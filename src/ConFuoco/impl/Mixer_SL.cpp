/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Definitions.h"
#ifdef RAINBOW_ANDROID

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "Common/IO.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Sound.h"

namespace ConFuoco
{
	Mixer *Mixer::Instance = nullptr;

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
			(*caller)->SetPlayState(caller, SL_PLAYSTATE_STOPPED);
			(*caller)->SetPlayState(caller, SL_PLAYSTATE_PLAYING);
		}

		void SetPlayState(SLPlayItf &itf, SLuint32 state)
		{
			(*itf)->SetPlayState(itf, state);
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
				if ((*this->engine_obj)->Realize(this->engine_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to realize SL engine\n");
					return;
				}
				if ((*this->engine_obj)->GetInterface(this->engine_obj, SL_IID_ENGINE, &this->engine) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to get interface for SL engine\n");
					return;
				}
				const SLInterfaceID iids[] = { SL_IID_ENVIRONMENTALREVERB };
				const SLboolean req[] = { SL_BOOLEAN_FALSE };
				if ((*this->engine)->CreateOutputMix(this->engine, &this->mix_obj, 1, iids, req) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to create SL output mix\n");
					return;
				}
				if ((*this->mix_obj)->Realize(this->mix_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to realize SL output mix\n");
					return;
				}
			}

			~SLContext()
			{
				if (this->mix_obj)
					(*this->mix_obj)->Destroy(this->mix_obj);
				if (this->engine_obj)
					(*this->engine_obj)->Destroy(this->engine_obj);
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

				if ((*context->engine)->CreateAudioPlayer(
						context->engine, &this->player, &source, &sink,
						1, iids, req) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to create audio player (%p)\n", stream);
					this->release();
					return false;
				}
				if ((*this->player)->Realize(this->player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
				{
					R_ERROR("[Rainbow::ConFuoco] Failed to realize audio player (%p)\n", stream);
					this->release();
					return false;
				}
				if ((*this->player)->GetInterface(this->player, SL_IID_PLAY, &this->play_itf) != SL_RESULT_SUCCESS)
					R_ERROR("[Rainbow::ConFuoco] Failed to get playback interface (%p)\n", stream);
				if (!this->play_itf)
				{
					this->release();
					return false;
				}
				if (stream->type == STREAM)
				{
					this->loops = stream->loops;
					if ((*this->play_itf)->SetCallbackEventsMask(this->play_itf, SL_PLAYEVENT_HEADATEND) != SL_RESULT_SUCCESS
					    || (*this->play_itf)->RegisterCallback(this->play_itf, PlayCallback, &this->loops) != SL_RESULT_SUCCESS)
					{
						this->release();
						return false;
					}
				}
				if ((*this->player)->GetInterface(this->player, SL_IID_VOLUME, &this->volume_itf) != SL_RESULT_SUCCESS)
					R_ERROR("[Rainbow::ConFuoco] Failed to get volume interface (%p)\n", stream);
				return true;
			}

			void release()
			{
				this->loops = -1;
				this->stream = nullptr;
				if (this->player)
				{
					(*this->player)->Destroy(this->player);
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

	Mixer::Mixer() : master_gain(1.0f), context(new SLContext())
	{
		for (size_t i = 0; i < num_channels; ++i)
		{
			this->channels[i].ch = i;
			this->channels[i].mixer = this;
		}
		Instance = this;
	}

	Mixer::~Mixer()
	{
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

		Rainbow::IO::FileHandle asset = Rainbow::IO::open(file, 0);
		if (!asset)
			return nullptr;

		off_t offset, length;
		int fd = AAsset_openFileDescriptor(asset, &offset, &length);
		Rainbow::IO::close(asset);

		SLDataLocator_AndroidFD android_fd = { SL_DATALOCATOR_ANDROIDFD, fd, offset, length };

		Stream *stream = new Stream(type, this, android_fd, loops);
		this->sounds.push_back(stream);
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
				return nullptr;

			s->channel = ch->ch;
			bool success = players[s->channel].load(static_cast<SLContext*>(this->context), s);
			if (!success)
				return nullptr;
		}
		if (!ch)
			return nullptr;

		ch->sound = snd;
		this->set_gain(ch, this->master_gain);
		this->play(ch);
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
				(*player.play_itf)->GetPlayState(player.play_itf, &state);
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
		(*itf)->GetPlayState(itf, &state);
		return players[ch->ch].paused || state == SL_PLAYSTATE_PLAYING;
	}

	void Mixer::set_gain(Channel *ch, const float gain)
	{
		SLVolumeItf &itf = players[ch->ch].volume_itf;
		(*itf)->SetVolumeLevel(itf, (1.0f - gain) * -100 * this->master_gain);
	}

	void Mixer::pause(Channel *ch)
	{
		SetPlayState(players[ch->ch].play_itf, SL_PLAYSTATE_PAUSED);
		players[ch->ch].paused = true;
	}

	void Mixer::play(Channel *ch)
	{
		SetPlayState(players[ch->ch].play_itf, SL_PLAYSTATE_PLAYING);
		players[ch->ch].paused = false;
	}

	void Mixer::stop(Channel *ch)
	{
		SetPlayState(players[ch->ch].play_itf, SL_PLAYSTATE_STOPPED);
		players[ch->ch].paused = false;
	}
}

#endif
