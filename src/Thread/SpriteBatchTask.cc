// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Graphics/SpriteBatch.h"
#include "Thread/SpriteBatchTask.h"

namespace Rainbow
{
	void SpriteBatchTask::end()
	{
		this->batch.upload();
	}

	void SpriteBatchTask::run()
	{
		this->batch.update();
	}
}
