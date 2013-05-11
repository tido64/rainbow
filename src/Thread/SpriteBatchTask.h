#ifndef THREAD_SPRITEBATCHTASK_H_
#define THREAD_SPRITEBATCHTASK_H_

#include "Thread/Task.h"

class SpriteBatch;

namespace Rainbow
{
	/// Task for updating a sprite batch and uploading its vertex data.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class SpriteBatchTask : public Task
	{
	public:
		inline SpriteBatchTask(SpriteBatch &batch);

	private:
		SpriteBatch &batch;

		virtual void end_impl() override;
		virtual void run_impl() override;
	};

	SpriteBatchTask::SpriteBatchTask(SpriteBatch &batch) : batch(batch) { }
}

#endif
