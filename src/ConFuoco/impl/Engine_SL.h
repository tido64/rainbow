#ifndef CONFUOCO_ENGINE_SL_H_
#define CONFUOCO_ENGINE_SL_H_

#include <cmath>
#include <SLES/OpenSLES.h>

#include "Common/Constants.h"
#include "Common/NonCopyable.h"

namespace ConFuoco
{
	/// Audio player powered by OpenSL.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	struct AudioPlayer
	{
		SLObjectItf player;
		SLPlayItf play_itf;
		SLVolumeItf volume_itf;

		inline AudioPlayer();
		inline ~AudioPlayer();

		inline SLuint32 get_state() const;
		inline void set_gain(const float gain);

		bool load(const char *const file);
		void release();

		inline void pause();
		inline void play();
		inline void stop();
	};

	/// Audio engine powered by OpenSL.
	///
	/// \see http://opensource.apple.com/source/file/file-23/file/magic/magic.mime
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Engine : public NonCopyable<Engine>
	{
		friend class Mixer;

	public:
		inline ~Engine();

		/// Create an audio player object.
		/// \return See SLresult.
		SLresult create_audio_player(SLObjectItf *, const char *const file);

		/// Return the mime type detected from \p header.
		/// \param header  4-bytes header.
		/// \return Container and mime type.
		SLDataFormat_MIME get_mimetype(const short *const header);

	private:
		SLObjectItf engine_obj;  ///< OpenSL engine object.
		SLEngineItf engine;      ///< OpenSL engine interface.

		SLObjectItf mix_obj;     ///< Output mix interface.

		Engine();

		inline bool is_available() const;

		inline void set_gain(const float gain) const;
		inline void set_orientation(const float r) const;
		inline void set_pitch(const float pitch) const;

		void shutdown();
	};

	AudioPlayer::AudioPlayer() :
		player(nullptr), play_itf(nullptr), volume_itf(nullptr) { }

	AudioPlayer::~AudioPlayer()
	{
		this->release();
	}

	SLuint32 AudioPlayer::get_state() const
	{
		SLuint32 state;
		(*this->play_itf)->GetPlayState(this->play_itf, &state);
		return state;
	}

	void AudioPlayer::set_gain(const float gain)
	{
		if (!this->volume_itf)
			return;

		(*this->volume_itf)->SetVolumeLevel(this->volume_itf, (1.0f - gain) * -100);
	}

	void AudioPlayer::pause()
	{
		if (!this->play_itf)
			return;

		(*this->play_itf)->SetPlayState(this->play_itf, SL_PLAYSTATE_PAUSED);
	}

	void AudioPlayer::play()
	{
		if (!this->play_itf)
			return;

		(*this->play_itf)->SetPlayState(this->play_itf, SL_PLAYSTATE_PLAYING);
	}

	void AudioPlayer::stop()
	{
		if (!this->play_itf)
			return;

		(*this->play_itf)->SetPlayState(this->play_itf, SL_PLAYSTATE_STOPPED);
	}

	Engine::~Engine()
	{
		this->shutdown();
	}

	bool Engine::is_available() const
	{
		return this->engine_obj && this->mix_obj;
	}

	void Engine::set_gain(const float) const { }
	void Engine::set_orientation(const float) const { }
	void Engine::set_pitch(const float) const { }
}

#endif
