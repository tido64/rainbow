#ifndef CONFUOCO_ENGINE_SL_H_
#define CONFUOCO_ENGINE_SL_H_

#include <cmath>
#include <SLES/OpenSLES.h>

#include "Common/Constants.h"

namespace ConFuoco
{
	/// Audio engine powered by OpenSL.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Engine
	{
		friend class Mixer;

	public:
		inline ~Engine();

	private:
		SLpermille pitch[2];     ///< Min and max pitch shift.
		SLEnvironmentalReverbSettings reverb;  ///< Environmental reverb settings.

		SLObjectItf engine_obj;  ///< OpenSL engine object.
		SLEngineItf engine;      ///< OpenSL engine interface.

		SLObjectItf mix_obj;     ///< Output mix interface.
		SLVolumeItf mix_volume;  ///< Audio volume interface.
		SLEnvironmentalReverbItf mix_reverb;
		SLPitchItf mix_pitch;    ///< Pitch shift interface.

		Engine();
		Engine(const Engine &);
		Engine& operator=(const Engine &);

		inline bool is_available() const;

		inline void set_gain(const float gain) const;
		inline void set_orientation(const float r) const;
		inline void set_pitch(const float pitch) const;

		void shutdown();
	};

	Engine::~Engine()
	{
		this->shutdown();
	}

	bool Engine::is_available() const
	{
		return this->engine_obj && this->mix_obj;
	}

	void Engine::set_gain(const float gain) const
	{
		(*this->mix_volume)->SetVolumeLevel(this->mix_volume, (1.0f - gain) * -100);
	}

	void Engine::set_orientation(const float) const
	{
		//float ori[] = { cosf(r + kPi_2), sinf(r + kPi_2), 0.0f, 0.0f, 0.0f, 1.0f };
		//alListenerfv(AL_ORIENTATION, ori);
	}

	void Engine::set_pitch(const float p) const
	{
		SLpermille pitch = this->pitch[0];
		if (p > 0.0f)
		{
			pitch += p * (this->pitch[1] - pitch);
			if (pitch > this->pitch[1])
				pitch = this->pitch[1];
		}
		(*this->mix_pitch)->SetPitch(this->mix_pitch, pitch);
	}
}

#endif
