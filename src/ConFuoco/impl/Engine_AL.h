#ifndef CONFUOCO_ENGINE_AL_H_
#define CONFUOCO_ENGINE_AL_H_

#include <cmath>

#ifdef RAINBOW_IOS
#	include <AudioToolbox/AudioServices.h>
#endif

#include "Common/Constants.h"
#include "ConFuoco/OpenAL.h"

namespace ConFuoco
{
	/// Audio engine powered by OpenAL.
	///
	/// Copyright 2011-2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Engine
	{
		friend class Mixer;

	public:
		~Engine();

	private:
	#ifdef RAINBOW_IOS
		static void InterruptionListener(void *, UInt32);
		static void RouteChangeListener(void *, AudioSessionPropertyID, UInt32, const void *);
	#endif

		ALCcontext *context;  ///< Active OpenAL context.

		Engine();
		Engine(const Engine &);
		Engine& operator=(const Engine &);

		inline bool is_available() const;

		inline void set_gain(const float gain) const;
		inline void set_orientation(const float r) const;
		inline void set_pitch(const float pitch) const;
	};

	bool Engine::is_available() const
	{
		return this->context;
	}

	void Engine::set_gain(const float gain) const
	{
		alListenerf(AL_GAIN, gain);
	}

	void Engine::set_orientation(const float r) const
	{
		float ori[] = { cosf(r + kPi_2), sinf(r + kPi_2), 0.0f, 0.0f, 0.0f, 1.0f };
		alListenerfv(AL_ORIENTATION, ori);
	}

	void Engine::set_pitch(const float pitch) const
	{
		alListenerf(AL_PITCH, (pitch < 0.01f) ? 0.01f : pitch);
	}
}

#endif
