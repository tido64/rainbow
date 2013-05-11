// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Graphics/SpriteBatch.h"
#include "Thread/SpriteBatchTask.h"

namespace Rainbow
{
	void SpriteBatchTask::end_impl()
	{
		this->batch.upload();
	}

	void SpriteBatchTask::run_impl()
	{
		this->batch.update();
	}
}
