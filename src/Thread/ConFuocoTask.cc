// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Mixer.h"
#include "Thread/ConFuocoTask.h"

namespace Rainbow
{
	void ConFuocoTask::run()
	{
		this->mixer.update();
	}
}
