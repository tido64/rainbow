#ifndef CONFUOCO_SOUND_AL_H_
#define CONFUOCO_SOUND_AL_H_

#include "ConFuoco/impl/Wave_AL.h"

namespace ConFuoco
{
	/// Buffered wave.
	///
	/// Loads a whole wave into buffer.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Sound : public AL::Wave
	{
	public:
		Sound();
		virtual ~Sound();
		virtual bool load(const char *const file);

	private:
		unsigned int bid;  ///< alBuffer id.
	};
}

#endif
