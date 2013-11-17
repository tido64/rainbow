// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ConFuoco/Mixer.h"
#ifdef RAINBOW_OS_ANDROID

#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>

#include "ConFuoco/Sound.h"
#include "FileSystem/File.h"

#define CF_TAG "[Rainbow::ConFuoco/SL] "

#define slCreateAudioPlayer(self, player, source, sink, interfaces, iids, req) \
	(*self)->CreateAudioPlayer(self, player, source, sink, interfaces, iids, req)
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

namespace
{
	/// Support looping a given number of times. The SL_IID_SEEK interface
	/// currently does not support this.
	void PlayEventCallback(SLPlayItf caller, void *pContext, SLuint32)
	{
		int &loops = *static_cast<int*>(pContext);
		if (loops == 0)
			return;
		if (loops > 0)
			--loops;
		slSetPlayState(caller, SL_PLAYSTATE_STOPPED);
		slSetPlayState(caller, SL_PLAYSTATE_PLAYING);
	}

	struct Stream : public ConFuoco::Sound
	{
		const int loops;
		const int fd;
		const off_t offset;
		const off_t length;

		Stream(const int fd, const off_t offset, const off_t length, const int loops);
		virtual ~Stream();
	};
}

namespace ConFuoco
{
	MixerSL::MixerSL() : engine_obj(nullptr), engine(nullptr), output_mix(nullptr)
	{
		if (slCreateEngine(&this->engine_obj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to create SL engine\n");
			return;
		}
		if (slRealize(this->engine_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to realize SL engine\n");
			return;
		}
		if (slGetInterface(this->engine_obj, SL_IID_ENGINE, &this->engine) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to get interface for SL engine\n");
			return;
		}
		const SLInterfaceID iids[] = { SL_IID_ENVIRONMENTALREVERB };
		const SLboolean req[] = { SL_BOOLEAN_FALSE };
		if (slCreateOutputMix(this->engine, &this->output_mix, 1, iids, req) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to create SL output mix\n");
			return;
		}
		if (slRealize(this->output_mix, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to realize SL output mix\n");
			slDestroy(this->output_mix);
			this->output_mix = nullptr;
			return;
		}
		for (size_t i = 0; i < kNumChannels; ++i)
		{
			this->channels[i].ch = i;
			this->players[i].channel = &this->channels[i];
		}
		this->init();
	}

	MixerSL::~MixerSL()
	{
		if (this->engine_obj)
		{
			if (this->output_mix)
			{
				this->clear();
				slDestroy(this->output_mix);
			}
			slDestroy(this->engine_obj);
		}
	}

	void MixerSL::set_gain_impl(const float) { /* TODO */ }
	void MixerSL::set_pitch_impl(const float) { }

	void MixerSL::clear_impl()
	{
		for (auto &player : this->players)
			player.release();
		for (auto sound : this->sounds)
			delete sound;
		this->sounds.clear();
	}

	void MixerSL::suspend_impl(const bool suspend)
	{
		if (suspend)
		{
			for (auto &player : this->players)
			{
				if (player.state() == SL_PLAYSTATE_PLAYING)
				{
					player.suspended = true;
					player.set_state(SL_PLAYSTATE_PAUSED);
				}
			}
		}
		else
		{
			for (auto &player : this->players)
			{
				if (player.suspended)
				{
					player.suspended = false;
					player.set_state(SL_PLAYSTATE_PLAYING);
				}
			}
		}
	}

	void MixerSL::update_impl() { }

	/* Channel interface */

	bool MixerSL::is_paused_impl(const Channel *c)
	{
		return this->players[c->ch].state() == SL_PLAYSTATE_PAUSED;
	}

	bool MixerSL::is_playing_impl(const Channel *c)
	{
		return this->players[c->ch].state() == SL_PLAYSTATE_PLAYING;
	}

	void MixerSL::set_gain_impl(const Channel *c, const float gain)
	{
		slSetVolumeLevel(this->players[c->ch].volume_itf,
		                 (1.0f - gain) * -100 * this->master_gain);
	}

	void MixerSL::pause_impl(const Channel *c)
	{
		SLAudioPlayer &player = this->players[c->ch];
		const SLuint32 state = player.state();
		if (state == SL_PLAYSTATE_STOPPED)
			return;

		player.set_state((state == SL_PLAYSTATE_PAUSED) ? SL_PLAYSTATE_PLAYING
		                                                : SL_PLAYSTATE_PAUSED);
	}

	void MixerSL::play_impl(const Channel *c)
	{
		SLAudioPlayer &player = this->players[c->ch];
		player.load(this->engine, this->output_mix);
		player.set_state(SL_PLAYSTATE_PLAYING);
	}

	void MixerSL::stop_impl(const Channel *c)
	{
		this->players[c->ch].set_state(SL_PLAYSTATE_STOPPED);
	}

	/* Sound interface */

	Sound* MixerSL::create_sound_impl(const char *const file,
	                                  const Sound::Type,
	                                  const int loops)
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

		off_t offset, length;
		int fd = 0;
		{
			const File &asset = File::open_asset(file);
			if (!asset)
				return nullptr;
			fd = AAsset_openFileDescriptor(asset, &offset, &length);
		}
		Stream *stream = new Stream(fd, offset, length, loops);
		this->sounds.push_back(stream);
		return stream;
	}

	void MixerSL::release_impl(Sound *s)
	{
		for (auto &channel : this->channels)
		{
			if (channel.sound == s)
			{
				this->stop(&channel);
				this->players[channel.ch].release();
				channel.sound = nullptr;
			}
		}
	}

	MixerSL::SLAudioPlayer::SLAudioPlayer() :
		suspended(false), loops(0), channel(nullptr), player(nullptr),
		play_itf(nullptr), volume_itf(nullptr) { }

	MixerSL::SLAudioPlayer::~SLAudioPlayer()
	{
		this->release();
	}

	SLuint32 MixerSL::SLAudioPlayer::state() const
	{
		if (!this->play_itf)
			return SL_PLAYSTATE_STOPPED;

		SLuint32 state;
		slGetPlayState(this->play_itf, &state);
		return state;
	}

	void MixerSL::SLAudioPlayer::set_state(const SLuint32 state)
	{
		if (!this->play_itf)
			return;

		slSetPlayState(this->play_itf, state);
	}

	bool MixerSL::SLAudioPlayer::load(SLEngineItf engine, SLObjectItf mixer)
	{
		this->release();

		Stream *stream = static_cast<Stream*>(this->channel->sound);
		SLDataLocator_AndroidFD fd = { SL_DATALOCATOR_ANDROIDFD, stream->fd, stream->offset, stream->length };
		SLDataFormat_MIME format = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
		SLDataSource source = { &fd, &format };

		SLDataLocator_OutputMix output_mix = { SL_DATALOCATOR_OUTPUTMIX, mixer };
		SLDataSink sink = { &output_mix, nullptr };

		const SLInterfaceID iids[] = { SL_IID_VOLUME };
		const SLboolean req[] = { SL_BOOLEAN_TRUE };

		if (slCreateAudioPlayer(engine, &this->player, &source, &sink, 1, iids, req) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to create audio player (%p)\n", stream);
			this->release();
			return false;
		}
		if (slRealize(this->player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to realize audio player (%p)\n", stream);
			this->release();
			return false;
		}
		if (slGetInterface(this->player, SL_IID_PLAY, &this->play_itf) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to get playback interface (%p)\n", stream);
			this->release();
			return false;
		}
		if (stream->loops != 0)
		{
			this->loops = stream->loops;
			if (slSetCallbackEventsMask(this->play_itf, SL_PLAYEVENT_HEADATEND) != SL_RESULT_SUCCESS
			    || slRegisterCallback(this->play_itf, PlayEventCallback, &this->loops) != SL_RESULT_SUCCESS)
			{
				R_ERROR(CF_TAG "Failed to set up looping (%p)\n", stream);
				this->release();
				return false;
			}
		}
		if (slGetInterface(this->player, SL_IID_VOLUME, &this->volume_itf) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to get volume interface (%p)\n", stream);
			this->release();
			return false;
		}
		Instance->set_gain(this->channel, 1.0f);
		return true;
	}

	void MixerSL::SLAudioPlayer::release()
	{
		this->suspended = false;
		this->loops = 0;
		if (this->player)
		{
			slDestroy(this->player);
			this->player = nullptr;
			this->play_itf = nullptr;
			this->volume_itf = nullptr;
		}
	}

	MixerSL::SLAudioPlayer::operator bool() const
	{
		return this->player;
	}
}

Stream::Stream(const int fd, const off_t offset, const off_t length, const int loops) :
	ConFuoco::Sound(ConFuoco::Sound::Type::Stream), loops(loops), fd(fd),
	offset(offset), length(length) { }

Stream::~Stream()
{
	ConFuoco::Mixer::Instance->release(this);
}

#endif
