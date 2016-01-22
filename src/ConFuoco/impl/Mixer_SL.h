// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_IMPL_MIXER_SL_H_
#define CONFUOCO_IMPL_MIXER_SL_H_

#include <SLES/OpenSLES.h>

#include "ConFuoco/Mixer.h"

namespace ConFuoco
{
    class MixerSL : public MixerBase<MixerSL>
    {
        friend MixerBase<MixerSL>;

    public:
        MixerSL();
        ~MixerSL();

        inline SLEngineItf interface();

    private:
        struct SLAudioPlayer
        {
            bool suspended;
            int loops;

            const Channel *channel;
            SLObjectItf player;
            SLPlayItf play_itf;
            SLVolumeItf volume_itf;

            SLAudioPlayer();
            ~SLAudioPlayer();

            SLuint32 state() const;
            void set_state(const SLuint32 state);

            bool load(SLEngineItf engine, SLObjectItf output_mix);
            void release();

            explicit operator bool() const;
        };

        SLObjectItf engine_obj;  ///< OpenSL engine object.
        SLEngineItf engine;      ///< OpenSL engine interface.
        SLObjectItf output_mix;  ///< Output mix object.
        SLAudioPlayer players[kNumChannels];

        void set_gain_impl(const float gain);
        void set_pitch_impl(const float pitch);
        void clear_impl();
        void suspend_impl(const bool suspend);
        void update_impl();

        /* Channel interface */

        bool is_paused_impl(const Channel *c);
        bool is_playing_impl(const Channel *c);
        void set_gain_impl(const Channel *c, const float gain);
        void pause_impl(const Channel *c);
        void play_impl(const Channel *c);
        void stop_impl(const Channel *c);

        /* Sound interface */

        Sound* create_sound_impl(const char *const file,
                                 const Sound::Type type,
                                 const int loops);
        void release_impl(Sound *s);
    };

    inline SLEngineItf MixerSL::interface()
    {
        return this->engine;
    }

    using Mixer = MixerSL;
}

#endif
