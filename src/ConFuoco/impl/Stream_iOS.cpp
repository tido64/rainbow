// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_IOS

#include "Common/Debug.h"
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
		NSString *path = [NSString stringWithUTF8String:file];
		path = [[NSBundle mainBundle] pathForResource:[path stringByDeletingPathExtension] ofType:[path pathExtension]];
		this->player = [[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:&error];
		path = nil;
		if (!this->player)
		{
			NSLog(@"[Rainbow::ConFuoco] Stream: %@", [error description]);
			R_ASSERT(this->player, "Failed to create player");
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

	void Stream::pause()
	{
		[this->player pause];
	}

	void Stream::play(const float, const float, const float)
	{
		if (this->player.playing)
			return;

		[this->player play];
	}

	void Stream::stop()
	{
		[this->player stop];
	}
}

#endif
