// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_FMOD_STUDIO

#include "ConFuoco/impl/Mixer_FMOD.h"

namespace ConFuoco
{
	MixerFMOD *MixerFMOD::Instance = nullptr;

	MixerFMOD::MixerFMOD() : system(nullptr)
	{
		if (FMOD::Studio::System::create(&this->system) != FMOD_OK
		    || this->system->initialize(
		           32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr)
		       != FMOD_OK)
		{
			return;
		}
		Instance = this;
	}

	MixerFMOD::~MixerFMOD()
	{
		if (!this->system)
			return;

		Instance = nullptr;
		this->system->release();
	}
}

#endif
