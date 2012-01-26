// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_IOS

#include <cassert>

#include "ConFuoco/impl/Stream_iOS.h"

namespace ConFuoco
{
	Stream::~Stream()
	{
		this->release();
	}

	bool Stream::load(const char *const file)
	{
		this->release();

		NSError *error;
		NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:[NSString stringWithUTF8String:file]];
		this->player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:&error];
		[fileURL release];
		if (!this->player)
		{
			NSLog(@"Rainbow::ConFuoco::iOS::load: %@", [error description]);
			assert(this->player);
			return false;
		}

		this->player.numberOfLoops = -1;
		[this->player prepareToPlay];
		return true;
	}

	void Stream::set_gain(const float gain)
	{
		this->player.volume = gain;
	}

	void Stream::set_loops(const int loops)
	{
		this->player.numberOfLoops = loops;
	}

	void Stream::pause() const
	{
		[this->player pause];
	}

	void Stream::play(const float, const float, const float) const
	{
		if (this->player.playing)
			return;

		[this->player play];
	}

	void Stream::stop() const
	{
		[this->player stop];
	}
}

#endif
