// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Mixer.h"
#ifdef RAINBOW_ANDROID

#include "Common/RainbowAssert.h"

namespace ConFuoco
{
	Engine::Engine() :
		engine_obj(nullptr), engine(nullptr), mix_obj(nullptr),
		mix_volume(nullptr), mix_pitch(nullptr)
	{
		const SLInterfaceID iids[] = {
			SL_IID_VOLUME,
			SL_IID_ENVIRONMENTALREVERB,
			SL_IID_PITCH
		};
		const SLboolean req[] = {
			SL_BOOLEAN_TRUE,
			SL_BOOLEAN_FALSE,
			SL_BOOLEAN_TRUE
		};
		if (slCreateEngine(&this->engine_obj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS
		    || (*this->engine_obj)->Realize(this->engine_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS
		    || (*this->engine_obj)->GetInterface(this->engine_obj, SL_IID_ENGINE, &this->engine) != SL_RESULT_SUCCESS
		    || (*this->engine)->CreateOutputMix(this->engine, &this->mix_obj, 3, iids, req) != SL_RESULT_SUCCESS
		    || (*this->mix_obj)->Realize(this->mix_obj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			this->shutdown();
			return;
		}

		SLresult result = (*this->mix_obj)->GetInterface(this->mix_obj, SL_IID_VOLUME, &this->mix_volume);
		R_ASSERT(result == SL_RESULT_SUCCESS, "Failed to get audio volume interface");

		result = (*this->mix_obj)->GetInterface(this->mix_obj, SL_IID_PITCH, &this->mix_pitch);
		R_ASSERT(result == SL_RESULT_SUCCESS, "Failed to get pitch shift interface");
		(*this->mix_pitch)->GetPitchCapabilities(this->mix_pitch, this->pitch, this->pitch + 1);

		return;  // We won't implement reverb now.

		// Get the environmental reverb interface. This could fail if the
		// environmental reverb effect is not available, either because the
		// feature is not present, excessive CPU load, or the required
		// MODIFY_AUDIO_SETTINGS permission was not requested and granted.
		if ((*this->mix_obj)->GetInterface(this->mix_obj, SL_IID_ENVIRONMENTALREVERB, &this->mix_reverb) != SL_RESULT_SUCCESS)
			return;
		(*this->mix_reverb)->SetEnvironmentalReverbProperties(this->mix_reverb, &this->reverb);
	}

	void Engine::shutdown()
	{
		if (this->mix_obj)
		{
			(*this->mix_obj)->Destroy(this->mix_obj);
			this->mix_obj = nullptr;
			this->mix_volume = nullptr;
			this->mix_reverb = nullptr;
			this->mix_pitch = nullptr;
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
