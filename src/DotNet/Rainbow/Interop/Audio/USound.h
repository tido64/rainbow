// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_AUDIO_USOUND_H_
#define DOTNET_RAINBOW_INTEROP_AUDIO_USOUND_H_

#include "Audio/Mixer.h"
#include "DotNet/Rainbow/Interop/Helper.h"

namespace rainbow
{
    struct USound
    {
        using Sound = audio::Sound;

        uint32_t size_of_struct = sizeof(USound);
        /*StdCall*/ void (*dtor_fn)(Sound*) = &audio::release;
    };
}

#endif
