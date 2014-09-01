// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_IMPL_MIXER_FMOD_H_
#define CONFUOCO_IMPL_MIXER_FMOD_H_

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#	pragma GCC diagnostic ignored "-Wunused-function"
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <fmod_studio.hpp>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

#include "Common/NonCopyable.h"

namespace ConFuoco
{
	class MixerFMOD : private NonCopyable<MixerFMOD>
	{
	public:
		static MixerFMOD *Instance;

		MixerFMOD();
		~MixerFMOD();

		void suspend(const bool suspend);
		inline void update();

		inline FMOD::Studio::System* operator->() const;

	private:
		FMOD::Studio::System *system_;
	};

	FMOD::Studio::System* MixerFMOD::operator->() const
	{
		return system_;
	}

	void MixerFMOD::update()
	{
		system_->update();
	}

	using Mixer = MixerFMOD;
}

#endif
