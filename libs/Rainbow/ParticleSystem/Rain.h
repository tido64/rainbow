/// Rain system (or snow...:/)

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen


#ifndef RAIN_H_
#define RAIN_H_

#include "../Hardware/Screen.h"
#include "../Algorithm.h"
#include "ParticleSystem.h"

namespace Rainbow
{
	namespace Rain
	{
		struct ParticleActionPolicy
		{
			void operator()(PointSprite *, const unsigned int count, const unsigned int elapsed);
		};

		struct ParticleInitPolicy
		{
			static void init(PointSprite *);
			void operator()(PointSprite *, const unsigned int count);
		};
	}
}

typedef ParticleSystem<Rainbow::Rain::ParticleInitPolicy, Rainbow::Rain::ParticleActionPolicy, 500> RainPS;

#endif
