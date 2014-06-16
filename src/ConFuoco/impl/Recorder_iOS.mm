// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ConFuoco/Recorder.h"
#ifndef USE_FMOD_STUDIO

#include <AVFoundation/AVAudioRecorder.h>
#include <AVFoundation/AVAudioSession.h>
#include <CoreAudio/CoreAudioTypes.h>

#include "ConFuoco/impl/Mixer_AL.h"

#define kTemporaryFilePath [NSTemporaryDirectory() \
	stringByAppendingPathComponent:@"temp.bifrostentertainment.rainbow"]

namespace
{
	const unsigned long long kFileSizeThreshold = 8 * 1024 * 1024;
}

namespace ConFuoco
{
	RecorderiOS::RecorderiOS() : recorder(nullptr)
	{
		AudioSession *session = Mixer::Instance->session();
		[session setCategory:AVAudioSessionCategoryPlayAndRecord];

		NSString *path = kTemporaryFilePath;
		if ([[NSFileManager defaultManager] fileExistsAtPath:path])
			[[NSFileManager defaultManager] removeItemAtPath:path error:nil];
		NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithInt:kAudioFormatLinearPCM],  AVFormatIDKey,
			[NSNumber numberWithFloat:44100.0],              AVSampleRateKey,
			[NSNumber numberWithInt:1],                      AVNumberOfChannelsKey,
			[NSNumber numberWithInt:16],                     AVLinearPCMBitDepthKey,
			[NSNumber numberWithInt:AVAudioQualityHigh],     AVEncoderAudioQualityKey,
			nil];
		NSError *error;
		this->recorder = [[AVAudioRecorder alloc]
				initWithURL:[NSURL fileURLWithPath:path isDirectory:NO]
				   settings:settings
				      error:&error];
		if (!this->recorder)
		{
			NSLog(@"%@", error);
			return;
		}
		[this->recorder setMeteringEnabled:YES];
		[this->recorder prepareToRecord];
	}

	void RecorderiOS::pause_impl()
	{
		[this->recorder pause];
	}

	bool RecorderiOS::record_impl(const unsigned long duration)
	{
		return (duration) ? [this->recorder recordForDuration:duration]
		                  : [this->recorder record];
	}

	void RecorderiOS::stop_impl()
	{
		[this->recorder stop];
		[[NSFileManager defaultManager] removeItemAtPath:kTemporaryFilePath
		                                           error:nil];
	}

	void RecorderiOS::update_impl()
	{
		if (!this->recorder)
			return;

		NSError *error = nil;
		NSDictionary *attributes = [[NSFileManager defaultManager]
				attributesOfItemAtPath:kTemporaryFilePath
				                 error:&error];
		if (attributes.fileSize >= kFileSizeThreshold)
		{
			this->stop();
			this->record();
			return;
		}
		[this->recorder updateMeters];
		this->average = [this->recorder averagePowerForChannel:0];
		this->peak = [this->recorder peakPowerForChannel:0];
	}
}

#endif  // !USE_FMOD_STUDIO
