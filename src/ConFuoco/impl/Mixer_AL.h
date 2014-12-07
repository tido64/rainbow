// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_IMPL_MIXER_AL_H_
#define CONFUOCO_IMPL_MIXER_AL_H_

#include "ConFuoco/Mixer.h"

using ALCcontext = struct ALCcontext_struct;

#ifdef RAINBOW_OS_IOS

@interface AudioSession : NSObject
- (BOOL)setCategory:(NSString*)category;
@end

#endif

namespace ConFuoco
{
	class MixerAL : public MixerBase<MixerAL>
	{
		friend MixerBase<MixerAL>;

	public:
		MixerAL();
		~MixerAL();

#ifdef RAINBOW_OS_IOS
		AudioSession* session() const { return this->session_; }
#endif

	private:
		ALCcontext *context_;
#ifdef RAINBOW_OS_IOS
		AudioSession *session_;
#endif

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

	using Mixer = MixerAL;
}

#endif
