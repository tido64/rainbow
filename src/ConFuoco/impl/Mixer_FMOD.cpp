// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_FMOD_STUDIO

#include "ConFuoco/impl/Mixer_FMOD.h"

namespace ConFuoco
{
    MixerFMOD *MixerFMOD::Instance = nullptr;

    MixerFMOD::MixerFMOD() : system_(nullptr)
    {
        if (FMOD::Studio::System::create(&system_) != FMOD_OK
            || system_->initialize(
                   32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr)
               != FMOD_OK)
        {
            return;
        }
        Instance = this;
    }

    MixerFMOD::~MixerFMOD()
    {
        if (!system_)
            return;

        Instance = nullptr;
        system_->release();
    }

    void MixerFMOD::suspend(const bool suspend)
    {
        FMOD::System *system;
        if (system_->getLowLevelSystem(&system) != FMOD_OK)
            return;
        if (suspend)
            system->mixerSuspend();
        else
            system->mixerResume();
    }
}

#endif
