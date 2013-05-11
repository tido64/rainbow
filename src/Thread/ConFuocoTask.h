#ifndef THREAD_CONFUOCOTASK_H_
#define THREAD_CONFUOCOTASK_H_

#include "Thread/Task.h"

namespace ConFuoco
{
	class Mixer;
}

namespace Rainbow
{
	/// Task for updating audio streams.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class ConFuocoTask : public Task
	{
	public:
		inline ConFuocoTask(ConFuoco::Mixer &mixer);
		virtual void run() override;

	private:
		ConFuoco::Mixer &mixer;
	};

	ConFuocoTask::ConFuocoTask(ConFuoco::Mixer &mixer) : mixer(mixer) { }
}

#endif
