// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include "Common/Debug.h"
#include "Common/IO.h"
#include "ConFuoco/Mixer.h"

#include <SLES/OpenSLES_Android.h>

namespace ConFuoco
{
	bool AudioPlayer::load(const char *const file)
	{
		if (Mixer::Instance().engine.create_audio_player(&this->player, file) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to create audio player for '%s'\n", file);
			return false;
		}
		if ((*this->player)->Realize(this->player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			this->release();
			R_ERROR("[Rainbow::ConFuoco] Failed to realize audio player for '%s'\n", file);
			return false;
		}
		if ((*this->player)->GetInterface(this->player, SL_IID_PLAY, &this->play_itf) != SL_RESULT_SUCCESS)
		{
			this->release();
			R_ERROR("[Rainbow::ConFuoco] Failed to get playback interface for '%s'\n", file);
			return false;
		}
		if ((*this->player)->GetInterface(this->player, SL_IID_VOLUME, &this->volume_itf) != SL_RESULT_SUCCESS)
		{
			this->release();
			R_ERROR("[Rainbow::ConFuoco] Failed to get volume interface for '%s'\n", file);
			return false;
		}
		return true;
	}

	void AudioPlayer::release()
	{
		if (this->player)
		{
			this->stop();
			(*this->player)->Destroy(this->player);
			this->player = nullptr;
			this->play_itf = nullptr;
			this->volume_itf = nullptr;
		}
	}

	SLresult Engine::create_audio_player(SLObjectItf *player, const char *const file)
	{
		/**
		 * Disabled because creating an audio player with SLDataLocator_URI
		 * doesn't seem to work. Android implements OpenSL 1.0.1 *partially*
		 * and has _extensions_ instead.
		 *
		char asset_uri[256] = "file:///android_asset/";
		strncpy(asset_uri, file, 232);
		SLDataLocator_URI uri = { SL_DATALOCATOR_URI, reinterpret_cast<SLchar*>(asset_uri) };
		SLDataFormat_MIME format = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
		SLDataSource source = { &uri, &format };
		 */

		Rainbow::IO::FileHandle asset = Rainbow::IO::open(file, 0);
		if (!asset)
			return SL_RESULT_CONTENT_NOT_FOUND;

		off_t offset, length;
		int fd = AAsset_openFileDescriptor(asset, &offset, &length);
		Rainbow::IO::close(asset);

		SLDataLocator_AndroidFD android_fd = { SL_DATALOCATOR_ANDROIDFD, fd, offset, length };
		SLDataFormat_MIME format = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
		SLDataSource source = { &android_fd, &format };

		SLDataLocator_OutputMix output_mix = { SL_DATALOCATOR_OUTPUTMIX, this->mix_obj };
		SLDataSink sink = { &output_mix, nullptr };

		const SLInterfaceID iids[] = { SL_IID_VOLUME };
		const SLboolean req[] = { SL_BOOLEAN_TRUE };

		return (*this->engine)->CreateAudioPlayer(
			this->engine, player, &source, &sink, 1, iids, req);
	}

	/**
	 * Only used when creating an audio player with SLDataLocator_URI.
	 *
	SLDataFormat_MIME Engine::get_mimetype(const short *const header)
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
	 */

	Engine::Engine() :
		engine_obj(nullptr), engine(nullptr), mix_obj(nullptr)
	{
		if (slCreateEngine(&this->engine_obj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
		{
			R_ERROR("[Rainbow::ConFuoco] Failed to create SL engine\n");
			return;
		}
		if ((*this->engine_obj)->Realize(this->engine_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			this->shutdown();
			R_ERROR("[Rainbow::ConFuoco] Failed to realize SL engine\n");
			return;
		}
		if ((*this->engine_obj)->GetInterface(this->engine_obj, SL_IID_ENGINE, &this->engine) != SL_RESULT_SUCCESS)
		{
			this->shutdown();
			R_ERROR("[Rainbow::ConFuoco] Failed to get interface for SL engine\n");
			return;
		}
		const SLInterfaceID iids[] = { SL_IID_ENVIRONMENTALREVERB };
		const SLboolean req[] = { SL_BOOLEAN_FALSE };
		if ((*this->engine)->CreateOutputMix(this->engine, &this->mix_obj, 1, iids, req) != SL_RESULT_SUCCESS)
		{
			this->shutdown();
			R_ERROR("[Rainbow::ConFuoco] Failed to create SL output mix\n");
			return;
		}
		if ((*this->mix_obj)->Realize(this->mix_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			this->shutdown();
			R_ERROR("[Rainbow::ConFuoco] Failed to realize SL output mix\n");
			return;
		}
	}

	void Engine::shutdown()
	{
		if (this->mix_obj)
		{
			(*this->mix_obj)->Destroy(this->mix_obj);
			this->mix_obj = nullptr;
		}
		if (this->engine_obj)
		{
			(*this->engine_obj)->Destroy(this->engine_obj);
			this->engine_obj = nullptr;
			this->engine = nullptr;
		}
	}
}

#endif
